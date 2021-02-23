#include "../Main.hpp"
#include "ELFLoader.hpp"

 struct OrbisELFLoader_header
{
    uint32_t magic;
    uint64_t entry;

    uint64_t thr_initial;
    uint8_t ShellCodeComplete;
    uint64_t ELFEntryPoint;
}__attribute__((packed));

static const char ElfMagic[] = { 0x7f, 'E', 'L', 'F', '\0' };
extern uint8_t OrbisELFLoader[];
extern int32_t OrbisELFLoaderSize;

int proc_create_thread(proc *proc, uint64_t address) 
{
    size_t n;
    int err = 0;
    uint64_t thr_initial = 0;
    uint8_t ShellCodeComplete = 0;
    void* gShellCodePtr = NULL;
    void* gStackPtr = NULL;
    size_t gShellCodeSize = 0;

    if(proc == NULL)
    {
        DebugLog(LOGTYPE_ERR, "Proc was NULL.");
        return err;
    }

    gShellCodeSize = OrbisELFLoaderSize;
	gShellCodeSize += (PAGE_SIZE - (gShellCodeSize % PAGE_SIZE));
	if(proc_allocate(proc, &gShellCodePtr, gShellCodeSize)) 
    {
        DebugLog(LOGTYPE_ERR, "Failed to allocate ShellCode Memory.");
        return err;
    }

    size_t StackSize = 0x80000;
	if(proc_allocate(proc, &gStackPtr, StackSize)) {
        DebugLog(LOGTYPE_ERR, "Failed to allocate Stack Memory.");

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

        return err;
    }

    err = proc_rw_mem(proc, gShellCodePtr, OrbisELFLoaderSize, (void *)OrbisELFLoader, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write Shellcode to Memory. Error: %d.", err);

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return err;
    }

    if(proc->p_dynlibptr == NULL) 
    {
        DebugLog(LOGTYPE_ERR, "p_dynlibptr is NULL.");

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return err;
    }

    dynlib* m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        if(!strcmp(basename(m_library->ModulePath), "libkernel.sprx"))
			thr_initial = (uint64_t)m_library->codeBase + addr_thr_initial_libkernel;

        if(!strcmp(basename(m_library->ModulePath), "libkernel_web.sprx"))
			thr_initial = (uint64_t)m_library->codeBase + addr_thr_initial_libkernel_web;

        if(!strcmp(basename(m_library->ModulePath), "libkernel_sys.sprx"))
			thr_initial = (uint64_t)m_library->codeBase + addr_thr_initial_libkernel_sys;

        //TODO: Remove or update to 6.72 if required...
        //if(!strcmp(basename(m_library->ModulePath), "mini-syscore.elf"))
		//	thr_initial = (uint64_t)m_library->codeBase + addr_thr_initial_minisyscore;

        m_library = m_library->dynlib_next;
    }

    if(thr_initial == 0) 
    {
		DebugLog(LOGTYPE_ERR, "Failed to resolve thr_initial.");

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return err;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisELFLoader_header, thr_initial), sizeof(thr_initial), (void *)&thr_initial, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write thr_initial to ShellCode. Error: %d.", err);

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return err;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisELFLoader_header, ELFEntryPoint), sizeof(address), (void *)&address, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write ELFEntryPoint to ShellCode. Error: %d.", err);

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return err;
    }

    struct thread *thr = TAILQ_FIRST(&proc->p_threads);
	uint64_t ShellCodeEntry = (uint64_t)gShellCodePtr + *(uint64_t *)(OrbisELFLoader + 4);
	create_thread(thr, NULL, (void*)ShellCodeEntry, NULL, (char*)gStackPtr, StackSize, NULL, NULL, NULL, 0, NULL);

    while (!ShellCodeComplete) 
	{
        err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisELFLoader_header, ShellCodeComplete), sizeof(ShellCodeComplete), (void *)&ShellCodeComplete, &n, 0);
        if(err)
        {
            DebugLog(LOGTYPE_ERR, "Failed to read ShellCodeComplete.");
            return err;
        }

        //DebugLog(LOGTYPE_INFO, "Waiting for ShellCode to compelete!");
        pause("", 100);
	}

    if (gShellCodePtr)
        proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

    if (gStackPtr)
        proc_deallocate(proc, gStackPtr, 0x80000);

    return err;
}

static inline Elf64_Phdr *elf_pheader(Elf64_Ehdr *hdr) {
	if (!hdr->e_phoff) {
		return NULL;
	}

	return (Elf64_Phdr *)((uint64_t)hdr + hdr->e_phoff);
}
static inline Elf64_Phdr *elf_segment(Elf64_Ehdr *hdr, int idx) {
	uint64_t addr = (uint64_t)elf_pheader(hdr);
	if (!addr) {
		return NULL;
	}

	return (Elf64_Phdr *)(addr + (hdr->e_phentsize * idx));
}
static inline Elf64_Shdr *elf_sheader(Elf64_Ehdr *hdr) {
	if (!hdr->e_shoff) {
		return NULL;
	}

	return (Elf64_Shdr *)((uint64_t)hdr + hdr->e_shoff);
}
static inline Elf64_Shdr *elf_section(Elf64_Ehdr *hdr, int idx) {
	uint64_t addr = (uint64_t)elf_sheader(hdr);
	if (!addr) {
		return NULL;
	}

	return (Elf64_Shdr *)(addr + (hdr->e_shentsize * idx));
}

int elf_mapped_size(void *elf, uint64_t *msize) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)elf;

    if (memcmp(ehdr->e_ident, ElfMagic, 4)) {
        return 1;
    }

    uint64_t s = 0;
    Elf64_Phdr *phdr = elf_pheader(ehdr);
    if (phdr) {
        for (int i = 0; i < ehdr->e_phnum; i++) {
            Elf64_Phdr *phdr = elf_segment(ehdr, i);

            uint64_t delta = phdr->p_paddr + phdr->p_memsz;
            if (delta > s) {
                s = delta;
            }
        }
    } else {
        for (int i = 0; i < ehdr->e_shnum; i++) {
            Elf64_Shdr *shdr = elf_section(ehdr, i);

            uint64_t delta = shdr->sh_addr + shdr->sh_size;
            if (delta > s) {
                s = delta;
            }
        }
    }

    if (msize) {
        *msize = s;
    }

    return 0;
}

int proc_map_elf(struct proc *p, void *elf, void *exec) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)elf;

    Elf64_Phdr *phdr = elf_pheader(ehdr);
    if (phdr) {
        for (int i = 0; i < ehdr->e_phnum; i++) {
            Elf64_Phdr *phdr = elf_segment(ehdr, i);

            if (phdr->p_filesz) {
                proc_write_mem(p, (void *)((uint8_t *)exec + phdr->p_paddr), phdr->p_filesz, (void *)((uint8_t *)elf + phdr->p_offset), NULL);
            }
        }
    } else {
        for (int i = 0; i < ehdr->e_shnum; i++) {
            Elf64_Shdr *shdr = elf_section(ehdr, i);

            if (!(shdr->sh_flags & SHF_ALLOC)) {
                continue;
            }

            if (shdr->sh_size) {
                proc_write_mem(p, (void *)((uint8_t *)exec + shdr->sh_addr), shdr->sh_size, (void *)((uint8_t *)elf + shdr->sh_offset), NULL);
            }
        }
    }

    return 0;
}

int proc_relocate_elf(struct proc *p, void *elf, void *exec) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)elf;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        Elf64_Shdr *shdr = elf_section(ehdr, i);


        if (shdr->sh_type == SHT_REL) {
            for (int j = 0; j < shdr->sh_size / shdr->sh_entsize; j++) {
                Elf64_Rela *reltab = &((Elf64_Rela *)((uint64_t)ehdr + shdr->sh_offset))[j];
                uint8_t **ref = (uint8_t **)((uint8_t *)exec + reltab->r_offset);
                uint8_t *value = NULL;

                switch (ELF64_R_TYPE(reltab->r_info)) {
                case R_X86_64_RELATIVE:
                    value = (uint8_t *)exec + reltab->r_addend;
                    proc_write_mem(p, ref, sizeof(value), (void *)&value, NULL);
                    break;
                case R_X86_64_64:
                case R_X86_64_JMP_SLOT:
                case R_X86_64_GLOB_DAT:
                    // not supported
                    break;
                }
            }
        }
    }

    return 0;
}

int proc_load_elf(struct proc *p, void *elf, uint64_t *elfbase, uint64_t *entry) {
    void *elfaddr = NULL;
    uint64_t msize = 0;
    int r = 0;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)elf;
    r = elf_mapped_size(elf, &msize);
    if (r) {
        goto error;
    }

    msize += (PAGE_SIZE - (msize % PAGE_SIZE));
    r = proc_allocate(p, &elfaddr, msize);
    if (r) {
        goto error;
    }

    r = proc_map_elf(p, elf, elfaddr);
    if (r) {
        goto error;
    }

    r = proc_relocate_elf(p, elf, elfaddr);
    if (r) {
        goto error;
    }

    if (elfbase) {
        *elfbase = (uint64_t)elfaddr;
    }

    if (entry) {
        *entry = (Elf64_Addr)((Elf64_Addr)elfaddr + ehdr->e_entry);
    }

error:
    return r;
}

int proc_LoadELF(proc *p, char* elf) {
    proc_vm_map_entry *entries;
    uint64_t num_entries;
    uint64_t entry;

    if(proc_load_elf(p, elf, NULL, &entry)) {
        return 1;
    }
    
    //Set Text Segments as writeable.
    dynlib* m_library = p->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        proc_mprotect(p, (void *)m_library->codeBase, (void*)m_library->codeSize, VM_PROT_ALL);
        m_library = m_library->dynlib_next;
    }
    
    if(proc_create_thread(p, entry)) {
        return 1;
    }

    return 0;
}