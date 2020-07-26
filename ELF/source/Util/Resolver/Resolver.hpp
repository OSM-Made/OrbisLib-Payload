#pragma once

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
extern int (*kproc_create)(void (*func)(void *), void *arg, struct proc **newpp, int flags, int pages, const char *fmt, ...);
extern int (*kproc_exit)(int code);
//extern kproc_resume
//extern kproc_shutdown
//extern kproc_start
//extern kproc_suspend
//extern kproc_suspend_check
extern int (*kproc_kthread_add)(void (*func)(void *), void *arg, struct proc **procptr, struct thread **tdptr, int flags, int pages, char * procname, const char *fmt, ...);

extern int (*kthread_add)(void (*func)(void *), void *arg, struct proc *procp, struct thread **newtdpp, int flags, int pages, const char *fmt, ...);
extern void (*kthread_exit)(void);
//extern void (*kthread_suspend)(thread *td, int timo);
extern void (*kthread_suspend_check)(void);
extern void (*kthread_set_affinity)(const char *tdname, uint64_t prio, uint64_t cpuset, uint64_t unknown);

/* Proc */

void ResolveFunctions();