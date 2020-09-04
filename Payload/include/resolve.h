#pragma once

#include "types.h"

#define SOFTWARE_VERSION_505
#ifdef SOFTWARE_VERSION_505

#define addr_Xfast_syscall						0x1C0

#define addr_malloc								0x10E250 
#define addr_free								0x10E460 
#define addr_memcpy								0x1EA530
#define addr_memset								0x3205C0
#define addr_memcmp								0x50AC0
#define addr_kmem_alloc							0xFCC80

#define addr_disable_console_output             0x19ECEB0
#define addr_M_TEMP					        	0x14B4110
#define addr_kernel_map                         0x1AC60E0
#define addr_prison0                            0x10986A0
#define addr_rootvnode                          0x22C1A70

#endif

#ifdef SOFTWARE_VERSION_672

#define addr_Xfast_syscall						0x1C0

#define addr_malloc								0xD7A0
#define addr_free								0xD9A0
#define addr_memcpy								0x3C15B0
#define addr_memset								0x1687D0
#define addr_memcmp								0x207E40
#define addr_kmem_alloc							0x250730

#define addr_disable_console_output             0x1A6EB18
#define addr_M_TEMP					        	0x1540EB0
#define addr_kernel_map                         0x220DFC0
#define addr_prison0                            0x113E518
#define addr_rootvnode                          0x2300320

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