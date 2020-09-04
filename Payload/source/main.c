#include "main.h"

struct payloadArgs {
	uint64_t sycall;
	void *payload;
	size_t psize;
};

extern uint8_t OrbisLibElf[];
extern int32_t OrbisLibElfSize;

int install_payload(struct thread *td, uint64_t kernbase, void *payload, size_t psize) {
	vm_offset_t (*kmem_alloc)(vm_map_t map, vm_size_t size) = (void *)(kernbase + __kmem_alloc);
	vm_map_t kernel_map = *(vm_map_t *)(kernbase + __kernel_map);

	size_t msize = 0;
	if (elf_mapped_size(payload, &msize)) {
		return 1;
	}

	int s = (msize + 0x3FFFull) & ~0x3FFFull;
	void *payloadbase = (void*)kmem_alloc(kernel_map, s);
	if (!payloadbase) {
		return 1;
	}

	int r = 0;
	int (*payload_entry)(void *p);

	if ((r = load_elf(payload, psize, payloadbase, msize, (void **)&payload_entry))) {
		return r;
	}

	if (payload_entry(NULL)) {
		return 1;
	}

	return 0;
}

int install_orbis(struct thread *td, struct payloadArgs *args) {
	uint64_t kernbase = getkernbase();
	resolve(kernbase);

	//Jailbreak
	struct ucred* cred = td->td_proc->p_ucred;
	struct filedesc* fd = td->td_proc->p_fd;

	cred->cr_prison = *(void**)(kernbase + __prison0); //prison0
	fd->fd_rdir = *(void**)(kernbase + __rootvnode); //rootvnode
	fd->fd_jdir = *(void**)(kernbase + __rootvnode); //rootvnode

	cred->cr_uid = 0;
	cred->cr_ruid = 0;
	cred->cr_rgid = 0;
	cred->cr_groups[0] = 0;

	void *td_ucred = *(void**)(((char*)td) + 304);
  	*(uint64_t*)(((char*)td_ucred) + 96) = 0xffffffffffffffff;
  	*(uint64_t*)(((char*)td_ucred) + 88) = 0x3801000000000013;
	*(uint64_t*)(((char*)td_ucred) + 104) = 0xffffffffffffffff; 



	//TODO: make more modular
	//================================================================================================================================
	uint64_t CR0 = __readcr0();
	__writecr0(CR0 & ~CR0_WP);

	//Kernel Patches
	uint8_t *disable_console_output = (uint8_t*)(kernbase + __disable_console_output);
	*disable_console_output = 0;

	// flatz allow sys_dynlib_dlsym in all processes 5.05
	*(uint64_t*)(kernbase + 0x0237F3A) = 0x8B4890000001C1E9;
	
	//debug settings patches 5.05
	*(char*)(kernbase + 0x1CD0686) |= 0x14;
  	*(char*)(kernbase + 0x1CD06A9) |= 3;
  	*(char*)(kernbase + 0x1CD06AA) |= 1;
  	*(char*)(kernbase + 0x1CD06C8) |= 1;

	//debug menu error patches 5.05
  	*(uint32_t*)(kernbase + 0x4F9048) = 0;
  	*(uint32_t*)(kernbase + 0x4FA15C) = 0;

  	*(uint32_t*)(kernbase + 0x6A2700) = 0x90C3C031; //flatz disable pfs signature check 5.05
  	*(uint32_t*)(kernbase + 0x64B2B0) = 0x90C301B0; //flatz enable debug RIFs 5.05
  	*(uint32_t*)(kernbase + 0x64B2D0) = 0x90C301B0; //flatz enable debug RIFs 5.05

	*(uint8_t*)(kernbase + 0x1EA53D) = 0xEB; //patch memcpy first
	*(uint16_t*)(kernbase + 0x194875) = 0x9090; //patch ASLR
	*(uint8_t*)(kernbase + 0xFCD48) = VM_PROT_ALL; //patch kmem_alloc
	*(uint8_t*)(kernbase + 0xFCD56) = VM_PROT_ALL; //patch kmem_alloc
	*(uint8_t *)(kernbase + 0x7673E0) = 0xC3; //disable sysdump_perform_dump_on_fatal_trap

	memcpy((void*)(kernbase + 0x11730), "\x48\xC7\xC0\x01\x00\x00\x00\xC3", 8); //patch sceSblACMgrIsAllowedSystemLevelDebugging
	memcpy((void*)(kernbase + 0x117B0), "\x48\xC7\xC0\x01\x00\x00\x00\xC3", 8); //patch sceSblACMgrHasMmapSelfCapability
	memcpy((void*)(kernbase + 0x117C0), "\x48\xC7\xC0\x01\x00\x00\x00\xC3", 8); //patch sceSblACMgrIsAllowedToMmapSelf
	memcpy((void*)(kernbase + 0x13F03F), "\x31\xC0\x90\x90\x90", 5); //self patches
	memcpy((void*)(kernbase + 0x1A3C08), "\x90\x90\x90\x90\x90\x90", 6); //patch vm_map_protect check

	memcpy((void*)(kernbase + 0x30DE01), "\xE9\xD0\x00\x00\x00", 5); //remove all these bullshit checks from ptrace, by golden
	*(uint8_t*)(kernbase + 0x30D9AA) = 0xEB; //patch ptrace

	//memcpy((void*)(kernbase + 0x262591), "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 11); //sysctl write only patch
	//memcpy((void*)(kernbase + 0x2626DE), "\xf1\x89\xd8", 3);

	__writecr0(CR0);

	//================================================================================================================================


	return install_payload(td, kernbase, args->payload, args->psize);
}

int _main(void) 
{
	syscall(11, install_orbis, OrbisLibElf, OrbisLibElfSize);

	//resolveImports();
	
	//sceSysUtilSendNotificationRequest("OrbisLib v2.0 Loaded!\nMade by OSM", 1);
	return 0;
}