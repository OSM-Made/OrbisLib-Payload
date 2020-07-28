#pragma once

extern "C"
{
    #include <vm/vm.h>
    #include <vm/pmap.h>
    #include <vm/vm_map.h>
    #include <sys/uio.h>
}

#include "Resolver-505.hpp"
#include "Resolver-672.hpp"

extern uint8_t* gpKernelBase;

#ifndef resolve
#define resolve(x) ((void*)((uint8_t *)&gpKernelBase[x]))
#endif

/* STD Lib */
extern void *M_TEMP;
extern void *(*malloc)(unsigned long size, void *type, int flags);
extern void (*free)(void *addr, void *type);
extern void (*memcpy)(void *dst, const void *src, size_t len);
extern void *(*memset)(void *ptr, int value, size_t num);
extern int (*memcmp)(const void *ptr1, const void *ptr2, size_t num);
extern size_t (*strlen)(const char *str);
extern int (*strcpy)(char * str1, char * str2);
extern int (*strcmp)(const char * str1, const char * str2);
extern int (*sprintf)(char* dst, const char *fmt, ...);
extern int (*vsprintf)(char* dst, const char* fmt, va_list ap);
extern int (*vprintf)(const char *fmt, va_list arg);

/* Kproc */
extern int (*kproc_create)(void (*func)(void *), void *arg, proc **newpp, int flags, int pages, const char *fmt, ...);
extern int (*kproc_exit)(int code);
//extern kproc_resume
//extern kproc_shutdown
//extern kproc_start
//extern kproc_suspend
//extern kproc_suspend_check
extern int (*kproc_kthread_add)(void (*func)(void *), void *arg, proc **procptr, thread **tdptr, int flags, int pages, char * procname, const char *fmt, ...);
extern void (*pause)(const char *wmesg, int timo);
extern int (*kthread_add)(void (*func)(void *), void *arg, proc *procp, thread **newtdpp, int flags, int pages, const char *fmt, ...);
extern void (*kthread_exit)(void);
//extern void (*kthread_suspend)(thread *td, int timo);
extern void (*kthread_suspend_check)(void);
extern void (*kthread_set_affinity)(const char *tdname, uint64_t prio, uint64_t cpuset, uint64_t unknown);

/* Proc */
extern int (*proc_kill)(proc *p, char* why);
extern int (*proc_rwmem)(proc *p, uio *uio);

/* ptrace */
extern int (*kptrace)(thread * td, int req, int pid, void * addr, int data);
extern int (*kpsignal)(proc* proc, int sig);
extern int (*kwait)(thread *td, int wpid, int *status, int options, void *rusage);
extern int (*kDelay)(uint64_t time);

/* Virtual Memory */
extern vmspace *(*vmspace_acquire_ref)(proc* p);
extern void (*vmspace_free)(vmspace* vm);
extern void (*vm_map_lock_read)(vm_map* map);
extern void (*vm_map_unlock_read)(vm_map* map);
extern int (*vm_map_lookup_entry)(vm_map* map, uint64_t address, vm_map_entry **entries);
extern int (*vm_map_findspace)(vm_map* map, uint64_t start, uint64_t length, uint64_t *addr);
extern int (*vm_map_insert)(vm_map* map, uint64_t object, uint64_t offset, uint64_t start, uint64_t end, int prot, int max, int cow);
extern void (*vm_map_lock)(vm_map* map);
extern void (*vm_map_unlock)(vm_map* map);
extern int (*vm_map_delete)(vm_map* map, uint64_t start, uint64_t end);
extern int (*vm_map_protect)(vm_map* map, uint64_t start, uint64_t end, int new_prot, uint64_t set_max);

void ResolveFunctions();