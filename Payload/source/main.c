#include "main.h"

struct payloadArgs {
	uint64_t sycall;
	void *payload;
	size_t psize;
};

struct payload_info {
  	uint8_t* buffer;
  	size_t size;
};

struct payload_header {
  	uint64_t signature;
  	size_t entrypoint_offset;
};

struct install_payload_args {
  void* syscall_handler;
  struct payload_info* payload_info;
};

extern uint8_t OrbisLibElf[];
extern int32_t OrbisLibElfSize;

extern uint8_t OrbisHen[];
extern int32_t OrbisHenSize;

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
  	//*(uint32_t*)(kernbase + 0x14A63F1) = 0x9090000; //enable vr 5.05

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

	__writecr0(CR0);

	//================================================================================================================================


	return install_payload(td, kernbase, args->payload, args->psize);
}

int install_hen(struct thread *td, struct install_payload_args* args) {
	uint8_t* kernel_base = (uint8_t*)getkernbase();

		void (*pmap_protect)(void* pmap, uint64_t sva, uint64_t eva, uint8_t pr) = (void*)(kernel_base + 0x2E3090);
  	void *kernel_pmap_store = (void*)(kernel_base + 0x22CB570);

  	uint8_t* payload_data = args->payload_info->buffer;
  	size_t payload_size = args->payload_info->size;
  	struct payload_header* payload_header = (struct payload_header*)payload_data;
  	uint8_t* payload_buffer = (uint8_t*)&kernel_base[0xB5EF30];

  	if (!payload_data || payload_size < sizeof(payload_header) || payload_header->signature != 0x5041594C4F414458ull)
  	  	return -1;

  	uint64_t cr0 = __readcr0();
  	__writecr0(cr0 & ~(1 << 16));

  	memset(payload_buffer, 0, 0x4000);
  	memcpy(payload_buffer, payload_data, payload_size);

  	uint64_t sss = (uint64_t)payload_buffer & ~0x3FFF;
  	uint64_t eee = ((uint64_t)payload_buffer + payload_size + 0x3FFF) & ~0x3FFF;
  	kernel_base[0x2E30D4] = 0xEB;
  	pmap_protect(kernel_pmap_store, sss, eee, 7);
  	kernel_base[0x2E30D4] = 0x75;

  	__writecr0(cr0);

  	int (*payload_entrypoint)();
  	*((void**)&payload_entrypoint) = (void*)(&payload_buffer[payload_header->entrypoint_offset]);

  	return payload_entrypoint();
}

int _main(void) { //TODO: Make modular for porting to multiple software versions

	syscall(11, install_orbis, OrbisLibElf, OrbisLibElfSize);

	struct payload_info payload_info;
  	payload_info.buffer = (uint8_t*)OrbisHen;
  	payload_info.size = (size_t)OrbisHenSize;

	syscall(11, install_hen, &payload_info);

	resolveImports();
	
	sceSysUtilSendSystemNotificationWithText(222, "OrbisLib v2.0 Loaded!\nMade by OSM");
	return 0;
}