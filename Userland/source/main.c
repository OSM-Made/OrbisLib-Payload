#include "main.h"
#ifdef SOFTWARE_VERSION_505
#include "Patches-505.h"
#endif
#ifdef SOFTWARE_VERSION_672
#include "Patches-672.h"
#endif

struct payloadArgs {
	uint64_t sycall;
	void *payload;
	size_t psize;
};

extern uint8_t OrbisLibElf[];
extern int32_t OrbisLibElfSize;

int install_payload(struct thread *td, uint64_t kernbase, void *payload, size_t psize) {
	vm_offset_t (*kmem_alloc)(vm_map_t map, vm_size_t size) = (void *)(kernbase + addr_kmem_alloc);
	vm_map_t kernel_map = *(vm_map_t *)(kernbase + addr_kernel_map);

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

	cred->cr_prison = *(void**)(kernbase + addr_prison0); //prison0
	fd->fd_rdir = *(void**)(kernbase + addr_rootvnode); //rootvnode
	fd->fd_jdir = *(void**)(kernbase + addr_rootvnode); //rootvnode

	cred->cr_uid = 0;
	cred->cr_ruid = 0;
	cred->cr_rgid = 0;
	cred->cr_groups[0] = 0;

	void *td_ucred = *(void**)(((char*)td) + 304);
  	*(uint64_t*)(((char*)td_ucred) + 96) = 0xffffffffffffffff;
  	*(uint64_t*)(((char*)td_ucred) + 88) = 0x3801000000000013;
	*(uint64_t*)(((char*)td_ucred) + 104) = 0xffffffffffffffff; 

	DoPatches(kernbase);
	
	return install_payload(td, kernbase, args->payload, args->psize);
}

int _main(void) 
{
	syscall(11, install_orbis, OrbisLibElf, OrbisLibElfSize);
	
	return 0;
}