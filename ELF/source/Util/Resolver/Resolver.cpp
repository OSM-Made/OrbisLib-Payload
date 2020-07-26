#include "../../main.hpp"
#include "Resolver.hpp"

uint8_t* gpKernelBase = 0;

void *M_TEMP;
void *(*malloc)(unsigned long size, void *type, int flags);
void (*free)(void *addr, void *type);
void (*memcpy)(void *dst, const void *src, size_t len);
void *(*memset)(void *ptr, int value, size_t num);
int (*memcmp)(const void *ptr1, const void *ptr2, size_t num);
size_t (*strlen)(const char *str);
int (*strcpy)(char * str1, char * str2);
int (*strcmp)(const char * str1, const char * str2);
int (*sprintf)(char* dst, const char *fmt, ...);
int (*vsprintf)(char* dst, const char* fmt, va_list ap);
int (*vprintf)(const char *fmt, va_list arg);

int (*kproc_create)(void (*func)(void *), void *arg, struct proc **newpp, int flags, int pages, const char *fmt, ...);
int (*kproc_exit)(int code);
//kproc_resume
//kproc_shutdown
//kproc_start
//kproc_suspend
//kproc_suspend_check
int (*kproc_kthread_add)(void (*func)(void *), void *arg, struct proc **procptr, struct thread **tdptr, int flags, int pages, char * procname, const char *fmt, ...);

int (*kthread_add)(void (*func)(void *), void *arg, struct proc *procp, struct thread **newtdpp, int flags, int pages, const char *fmt, ...);
void (*kthread_exit)(void);
//void (*kthread_suspend)(thread *td, int timo);
void (*kthread_suspend_check)(void);
void (*kthread_set_affinity)(const char *tdname, uint64_t prio, uint64_t cpuset, uint64_t unknown);

void ResolveFunctions()
{
    //something = ()resolve();

    /* STD Lib */
    M_TEMP = resolve(addr_M_TEMP);
    malloc = (void*(*)(unsigned long size, void *type, int flags))resolve(addr_malloc);
    free = (void(*)(void *addr, void *type))resolve(addr_free);
    memcpy = (void(*)(void *dst, const void *src, size_t len))resolve(addr_memcpy);
    memset = (void*(*)(void *ptr, int value, size_t num))resolve(addr_memset);
    memcmp = (int(*)(const void *ptr1, const void *ptr2, size_t num))resolve(addr_memcmp);
    strlen = (size_t(*)(const char *str))resolve(addr_strlen);
    strcpy = (int(*)(char * str1, char * str2))resolve(addr_strcpy);
    strcmp = (int(*)(const char * str1, const char * str2))resolve(addr_strcmp);
    sprintf = (int(*)(char* dst, const char *fmt, ...))resolve(addr_sprintf);
    vsprintf = (int(*)(char* dst, const char* fmt, va_list ap))resolve(addr_vsprintf);
    vprintf = (int(*)(const char *fmt, va_list arg))resolve(addr_vprintf);

    /* Kproc */
    kproc_create = (int(*)(void (*func)(void *), void *arg, struct proc **newpp, int flags, int pages, const char *fmt, ...))resolve(addr_kproc_create);
    kproc_exit = (int(*)(int code))resolve(addr_kproc_exit);
    //kproc_resume
    //kproc_shutdown
    //kproc_start
    //kproc_suspend
    //kproc_suspend_check
    kproc_kthread_add = (int(*)(void (*func)(void *), void *arg, struct proc **procptr, struct thread **tdptr, int flags, int pages, char * procname, const char *fmt, ...))resolve(addr_kproc_kthread_add);

    kthread_add = (int(*)(void (*func)(void *), void *arg, struct proc *procp, struct thread **newtdpp, int flags, int pages, const char *fmt, ...))resolve(addr_kthread_add);
    kthread_exit = (void(*)(void))resolve(addr_kthread_exit);
    //kthread_suspend
    kthread_suspend_check = (void(*)(void))resolve(addr_kthread_suspend_check);
    kthread_set_affinity = (void(*)(const char *tdname, uint64_t prio, uint64_t cpuset, uint64_t unknown))resolve(addr_kthread_set_affinity);

    /* Proc */

}