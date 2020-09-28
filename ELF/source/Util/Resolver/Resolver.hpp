#pragma once

extern "C"
{
    #include <vm/vm.h>
    #include <vm/pmap.h>
    #include <vm/vm_map.h>
    #include <sys/uio.h>
    #include <sys/elf64.h>
    #include <sys/eventhandler.h>
}

#include "Resolver-505.hpp"
#include "Resolver-672.hpp"

#include "../Helpers/FakeSelfHelper.hpp"

extern uint8_t* gpKernelBase;

#ifndef resolve
#define resolve(x) ((void*)((uint8_t *)&gpKernelBase[x]))
#endif

/* STD Lib */
extern void *M_TEMP;
extern void* M_MOUNT;
extern void *(*malloc)(unsigned long size, void *type, int flags);
extern void (*free)(void *addr, void *type);
extern void (*memcpy)(void *dst, const void *src, size_t len);
extern void *(*memset)(void *ptr, int value, size_t num);
extern int (*memcmp)(const void *ptr1, const void *ptr2, size_t num);
extern size_t (*strlen)(const char *str);
extern int (*strcpy)(char * str1, char * str2);
extern char* (*strncpy)(char *destination, const char *source, size_t num);
extern int (*strcmp)(const char * str1, const char * str2);
extern int (*sprintf)(char* dst, const char *fmt, ...);
extern int (*snprintf)(char *str, size_t size, const char *format, ...);
extern int (*vsprintf)(char* dst, const char* fmt, va_list ap);
extern int (*vprintf)(const char *fmt, va_list arg);
extern int(*sscanf)(const char *str, const char *format, ...);
extern char *(*strdup)(const char *s, void* type);
extern char *(*realloc)(void *addr, unsigned long size, void* mtp, int flags);

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
extern int (*create_thread)(thread * td, uint64_t ctx, void* start_func, void *arg, char *stack_base, size_t stack_size, char *tls_base, long * child_tid, long * parent_tid, uint64_t flags, uint64_t rtp);

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

/*Mutex Locks*/
extern void (*mtx_init)(mtx *m, const char *name, const char *type, int opts);
extern void (*mtx_destroy)(mtx *mutex);
extern void (*mtx_lock_flags)(mtx *mutex, int flags);
extern void (*mtx_unlock_flags)(mtx *mutex, int flags);
extern void (*_mtx_lock_flags)(mtx *mutex, int flags, const char *file, int line);
extern void (*_mtx_unlock_flags)(mtx *mutex, int flags, const char *file, int line);

/* Fake Selfs */
extern int (*sceSblAuthMgrGetSelfInfo)(SelfContext* ctx, void *exInfo);
extern int (*sceSblAuthMgrIsLoadable2)(SelfContext* pSelfContext, SelfAuthInfo* pOldAuthInfo, int32_t pPathId, SelfAuthInfo* pNewAuthInfo);
extern void (*sceSblAuthMgrSmStart)(void**);
extern int (*sceSblAuthMgrVerifyHeader)(SelfContext* pSelfContext);
extern int (*sceSblServiceMailbox)(uint32_t pServiceId, void* pRequest, void* pResponse);

/* Critical Sections */
extern void (*EnterCriticalSection)();
extern void (*ExitCriticalSection)();

/* Event Resolving */
#ifdef SOFTWARE_VERSION_505
extern eventhandler_tag (*eventhandler_register)(eventhandler_list *list, const char *name, void *func, void *arg, int priority);
#endif
#ifdef SOFTWARE_VERSION_672
extern eventhandler_tag (*eventhandler_register)(eventhandler_list *list, const char *name, void *func, const char* unk, void *arg, int priority);
#endif
extern void (*eventhandler_deregister)(eventhandler_list* a, eventhandler_entry* b);
extern eventhandler_list* (*eventhandler_find_list)(const char *name);

/*
	process_exec
	process_exit

	system_suspend_phase1
	system_resume_phase1
	shutdown_pre_sync
*/
#ifdef SOFTWARE_VERSION_505
#define EVENTHANDLER_REGISTER(name, func, arg, priority)		\
	eventhandler_register(NULL, #name, func, arg, priority)
#endif
#ifdef SOFTWARE_VERSION_672
#define EVENTHANDLER_REGISTER(name, func, arg, priority)		\
	eventhandler_register(NULL, #name, func, "", arg, priority)
#endif


#define EVENTHANDLER_DEREGISTER(name, tag) 				\
do {									\
	struct eventhandler_list *_el;					\
									\
	if ((_el = eventhandler_find_list(#name)) != NULL)		\
		eventhandler_deregister(_el, tag);			\
} while(0)

/* FileIO */
extern int (*kern_open)(thread* td, char *path, int pathseg, int flags, int mode);
extern int (*kern_mkdir)(thread* td, char *path, int pathseg, int mode);

/* Kernel Misc */
extern int (*kernel_sysctlbyname)(thread *td, char *name, void *old, size_t *oldlenp, void *pnew, size_t newlen, size_t *retval, int flags);


void ResolveFunctions();