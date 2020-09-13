#pragma once

#include "types.h"
#include "../../ELF/source/Util/Settings.hpp"
#ifdef SOFTWARE_VERSION_505

#define addr_Xfast_syscall						0x1C0

#define addr_malloc								0x10E250 
#define addr_free								0x10E460 
#define addr_memcpy								0x1EA530
#define addr_memset								0x3205C0
#define addr_memcmp								0x50AC0
#define addr_kmem_alloc							0xFCC80

#define addr_M_TEMP					        	0x14B4110
#define addr_kernel_map                         0x1AC60E0
#define addr_prison0                            0x10986A0
#define addr_rootvnode                          0x22C1A70

//Patches
#define patch_disable_console_output            0x19ECEB0
#define patch_sys_dynlib_dlsym                  0x237F3A
#define patch_BootParam                         0x1CD0650                          
#define patch_dbg_Settings_0                    patch_BootParam + 0x36
#define patch_dbg_Settings_1                    patch_BootParam + 0x59
#define patch_dbg_Settings_2                    patch_BootParam + 0x5A
#define patch_dbg_Settings_3                    patch_BootParam + 0x78
#define patch_dbg_err_0                         0x4F9048
#define patch_dbg_err_1                         0x4FA15C
#define patch_memcpy                            0x1EA53D
#define patch_ASLR                              0x194875
#define patch_kmem_Alloc_0                      0xFCD48
#define patch_kmem_Alloc_1                      0xFCD56
#define patch_Disable_Core_Dump                 0x7673E0
#define patch_SystemLevelDebugging              0x11730
#define patch_MmapSelfCapability                0x117B0
#define patch_AllowedToMmapSelf                 0x117C0
#define patch_vm_map_protect                    0x1A3C08
#define patch_ptrace_0                          0x30DE01
#define patch_ptrace_1                          0x30D9AA


#endif

#ifdef SOFTWARE_VERSION_672

#define addr_Xfast_syscall						0x1C0

#define addr_malloc								0xD7A0
#define addr_free								0xD9A0
#define addr_memcpy								0x3C15B0
#define addr_memset								0x1687D0
#define addr_memcmp								0x207E40
#define addr_kmem_alloc							0x250730

#define addr_M_TEMP					        	0x1540EB0
#define addr_kernel_map                         0x220DFC0
#define addr_prison0                            0x113E518
#define addr_rootvnode                          0x2300320

//Patches
#define patch_disable_console_output            0x1A6EB18
#define patch_sys_dynlib_dlsym                  0x1D895A
#define patch_BootParam                         0x1BD7FD0
#define patch_dbg_Settings_0                    patch_BootParam + 0x36
#define patch_dbg_Settings_1                    patch_BootParam + 0x59
#define patch_dbg_Settings_2                    patch_BootParam + 0x5A
#define patch_dbg_Settings_3                    patch_BootParam + 0x78
#define patch_dbg_err_0                         0x507B09
#define patch_dbg_err_1                         0x508D5C
#define patch_memcpy                            0x3C15BD
#define patch_ASLR                              0x3CECE1
#define patch_kmem_Alloc_0                      0x2507F5
#define patch_kmem_Alloc_1                      0x250803
#define patch_Disable_Core_Dump                 0x784120
#define patch_SystemLevelDebugging              0x233BD0
#define patch_MmapSelfCapability                0x233C40
#define patch_AllowedToMmapSelf                 0x233C50
#define patch_vm_map_protect                    0x451DB8
#define patch_ptrace_0                          0x10FD22
#define patch_ptrace_1                          0x10F879

#endif

void *M_TEMP;

uint64_t getkernbase();
void *alloc(uint32_t size);
void dealloc(void *addr);

void *(*malloc)(unsigned long size, void *type, int flags);
void (*free)(void *addr, void *type);
void (*memcpy)(void *dst, const void *src, size_t len);
void *(*memset)(void * ptr, int value, size_t num);
int (*memcmp)(const void * ptr1, const void * ptr2, size_t num);

void resolve(uint64_t kernbase);