#pragma once

void DoPatches(uint64_t kernbase)
{
    uint64_t CR0 = __readcr0();
	__writecr0(CR0 & ~CR0_WP);

    //Kernel Patches
	uint8_t *disable_console_output = (uint8_t*)(kernbase + patch_disable_console_output);
	*disable_console_output = 0;

	// flatz allow sys_dynlib_dlsym in all processes 5.05
	*(uint64_t*)(kernbase + patch_sys_dynlib_dlsym) = 0x8B4890000001C1E9;
	
	//debug settings patches 5.05
	/*(char*)(kernbase + patch_dbg_Settings_0) |= 0x14;
  	*(char*)(kernbase + patch_dbg_Settings_1) |= 3;
  	*(char*)(kernbase + patch_dbg_Settings_2) |= 1;
  	*(char*)(kernbase + patch_dbg_Settings_3) |= 1;*/

	//debug menu error patches 5.05
  	*(uint32_t*)(kernbase + patch_dbg_err_0) = 0;
  	*(uint32_t*)(kernbase + patch_dbg_err_1) = 0;

	*(uint8_t*)(kernbase + patch_memcpy) = 0xEB; //patch memcpy first
	*(uint16_t*)(kernbase + patch_ASLR) = 0x9090; //patch ASLR
	*(uint8_t*)(kernbase + patch_kmem_Alloc_0) = VM_PROT_ALL; //patch kmem_alloc
	*(uint8_t*)(kernbase + patch_kmem_Alloc_1) = VM_PROT_ALL; //patch kmem_alloc
	*(uint8_t *)(kernbase + patch_Disable_Core_Dump) = 0xC3; //disable sysdump_perform_dump_on_fatal_trap

	memcpy((void*)(kernbase + patch_SystemLevelDebugging), "\x48\xC7\xC0\x01\x00\x00\x00\xC3", 8); //patch sceSblACMgrIsAllowedSystemLevelDebugging
	memcpy((void*)(kernbase + patch_MmapSelfCapability), "\x48\xC7\xC0\x01\x00\x00\x00\xC3", 8); //patch sceSblACMgrHasMmapSelfCapability
	memcpy((void*)(kernbase + patch_AllowedToMmapSelf), "\x48\xC7\xC0\x01\x00\x00\x00\xC3", 8); //patch sceSblACMgrIsAllowedToMmapSelf
	memcpy((void*)(kernbase + patch_vm_map_protect), "\x90\x90\x90\x90\x90\x90", 6); //patch vm_map_protect check

	memcpy((void*)(kernbase + patch_ptrace_0), "\xE9\xD0\x00\x00\x00", 5); //remove all these bullshit checks from ptrace, by golden
	*(uint8_t*)(kernbase + patch_ptrace_1) = 0xEB; //patch ptrace

    __writecr0(CR0);
}