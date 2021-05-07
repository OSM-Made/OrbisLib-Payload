#include "../../main.hpp"
#include "Resolver.hpp"

uint8_t* gpKernelBase = 0;

/* STD Lib */
void *M_TEMP;
void* M_MOUNT;
void *(*malloc)(unsigned long size, void *type, int flags);
void (*free)(void *addr, void *type);
void (*memcpy)(void *dst, const void *src, size_t len);
void *(*memset)(void *ptr, int value, size_t num);
int (*memcmp)(const void *ptr1, const void *ptr2, size_t num);
size_t (*strlen)(const char *str);
int (*strcpy)(char * str1, char * str2);
char* (*strncpy)(char *destination, const char *source, size_t num);
int (*strcmp)(const char * str1, const char * str2);
char* (*strstr)(const char * str1, const char * str2);
int (*sprintf)(char* dst, const char *fmt, ...);
int (*snprintf)(char *str, size_t size, const char *format, ...);
int (*vsprintf)(char* dst, const char* fmt, va_list ap);
int (*vprintf)(const char *fmt, va_list arg);
int(*sscanf)(const char *str, const char *format, ...);
char *(*strdup)(const char *s, void* type);
char *(*realloc)(void *addr, unsigned long size, void* mtp, int flags);
void(*kprintf)(const char* fmt, ...) = nullptr;

/* Kproc */
int (*kproc_create)(void (*func)(void *), void *arg, struct proc **newpp, int flags, int pages, const char *fmt, ...);
int (*kproc_exit)(int code);
//kproc_resume
//kproc_shutdown
//kproc_start
//kproc_suspend
//kproc_suspend_check
int (*kproc_kthread_add)(void (*func)(void *), void *arg, struct proc **procptr, struct thread **tdptr, int flags, int pages, char * procname, const char *fmt, ...);
void (*pause)(const char *wmesg, int timo);
int (*kthread_add)(void (*func)(void *), void *arg, struct proc *procp, struct thread **newtdpp, int flags, int pages, const char *fmt, ...);
void (*kthread_exit)(void);
//void (*kthread_suspend)(thread *td, int timo);
void (*kthread_suspend_check)(void);
void (*kthread_set_affinity)(const char *tdname, uint64_t prio, uint64_t cpuset, uint64_t unknown);

/* Proc */
int (*proc_kill)(proc *p, char* why);
int (*proc_rwmem)(proc *p, uio *uio);
int (*create_thread)(thread * td, uint64_t ctx, void* start_func, void *arg, char *stack_base, size_t stack_size, char *tls_base, long * child_tid, long * parent_tid, uint64_t flags, uint64_t rtp);

/* ptrace */
int (*kptrace)(thread * td, int req, int pid, void * addr, int data);
int (*kpsignal)(proc* proc, int sig);
int (*kwait)(thread *td, int wpid, int *status, int options, void *rusage);
int (*kDelay)(uint64_t time);

/* Virtual memory */
vmspace *(*vmspace_acquire_ref)(proc *p);
void (*vmspace_free)(vmspace* vm);
void (*vm_map_lock_read)(vm_map* map);
void (*vm_map_unlock_read)(vm_map* map);
int (*vm_map_lookup_entry)(vm_map* map, uint64_t address, vm_map_entry **entries);
int (*vm_map_findspace)(vm_map* map, uint64_t start, uint64_t length, uint64_t *addr);
int (*vm_map_insert)(vm_map* map, uint64_t object, uint64_t offset, uint64_t start, uint64_t end, int prot, int max, int cow);
void (*vm_map_lock)(vm_map* map);
void (*vm_map_unlock)(vm_map* map);
int (*vm_map_delete)(vm_map* map, uint64_t start, uint64_t end);
int (*vm_map_protect)(vm_map* map, uint64_t start, uint64_t end, int new_prot, uint64_t set_max);

/*Mutex Locks*/
void (*mtx_init)(mtx *m, const char *name, const char *type, int opts);
void (*mtx_destroy)(mtx *mutex);
void (*mtx_lock_flags)(mtx *mutex, int flags);
void (*mtx_unlock_flags)(mtx *mutex, int flags);
void (*_mtx_lock_flags)(mtx *mutex, int flags, const char *file, int line);
void (*_mtx_unlock_flags)(mtx *mutex, int flags, const char *file, int line);

/* Fake Selfs */
int (*sceSblAuthMgrGetSelfInfo)(SelfContext* ctx, void *exInfo);
int (*sceSblAuthMgrIsLoadable2)(SelfContext* pSelfContext, SelfAuthInfo* pOldAuthInfo, int32_t pPathId, SelfAuthInfo* pNewAuthInfo);
void (*sceSblAuthMgrSmStart)(void**);
int (*sceSblAuthMgrVerifyHeader)(SelfContext* pSelfContext);
int (*sceSblServiceMailbox)(uint32_t pServiceId, void* pRequest, void* pResponse);

/*Critical Sections*/
void (*EnterCriticalSection)();
void (*ExitCriticalSection)();

/* Event Handling */
#ifdef SOFTWARE_VERSION_505
eventhandler_tag (*eventhandler_register)(eventhandler_list *list, const char *name, void *func, void *arg, int priority);
#endif
#ifdef SOFTWARE_VERSION_672 || SOFTWARE_VERSION_702 //5.5X -> 7.02
eventhandler_tag (*eventhandler_register)(eventhandler_list *list, const char *name, void *func, const char* unk, void *arg, int priority);
#endif
void (*eventhandler_deregister)(eventhandler_list* a, eventhandler_entry* b);
eventhandler_list* (*eventhandler_find_list)(const char *name);

/* FileIO */
int (*kern_open)(thread* td, char *path, int pathseg, int flags, int mode);
int (*kern_mkdir)(thread* td, char *path, int pathseg, int mode);

/* Kernel Misc */ 
int (*kernel_sysctlbyname)(thread *td, char *name, void *old, size_t *oldlenp, void *pnew, size_t newlen, size_t *retval, int flags);
int (*kernel_ioctl)(thread *td, int fd, unsigned long request, ...);

/* Flash & NVS */
int (*icc_nvs_read)(uint32_t block, uint32_t offset, uint32_t size, uint8_t* value);
int (*icc_nvs_write)(uint32_t block, uint32_t offset, uint32_t size, uint8_t* value);

/* Registry */
int (*sceRegMgrGetStr)(uint64_t RegID, char* Value, int len);
int (*sceRegMgrSetStr)(uint64_t RegID, char* Value, int len);
int (*sceRegMgrGetInt)(uint64_t RegID, int32_t* Value);
int (*sceRegMgrSetInt)(uint64_t RegID, int32_t Value);
int (*sceRegMgrGetBin)(uint64_t RegID, char* Value, int size);
int (*sceRegMgrSetBin)(uint64_t RegID, char* Value, int size);

#define NATIVE_RESOLVE(_Ty) _Ty = (decltype(_Ty))(void*)((uint8_t *)&gpKernelBase[addr_ ## _Ty]);

bool ResolveFunctions()
{
    gpKernelBase = (uint8_t*)__readmsr(0xC0000082) - addr_Xfast_syscall;

    if (!gpKernelBase)
        return false;

    /* STD Lib */
    NATIVE_RESOLVE(M_TEMP);
    NATIVE_RESOLVE(M_MOUNT);
    NATIVE_RESOLVE(malloc);
    NATIVE_RESOLVE(free);
    NATIVE_RESOLVE(memcpy);
    NATIVE_RESOLVE(memset);
    NATIVE_RESOLVE(memcmp);
    NATIVE_RESOLVE(strlen);
    NATIVE_RESOLVE(strcpy);
    NATIVE_RESOLVE(strncpy);
    NATIVE_RESOLVE(strcmp);
    NATIVE_RESOLVE(strstr);
    NATIVE_RESOLVE(sprintf);
    NATIVE_RESOLVE(snprintf);
    NATIVE_RESOLVE(vsprintf);
    NATIVE_RESOLVE(vprintf);
    NATIVE_RESOLVE(sscanf);
    NATIVE_RESOLVE(strdup);
    NATIVE_RESOLVE(realloc);
    NATIVE_RESOLVE(kprintf);
    

    /* Kproc */
    NATIVE_RESOLVE(kproc_create);
    NATIVE_RESOLVE(kproc_exit);
    NATIVE_RESOLVE(kproc_kthread_add);
    NATIVE_RESOLVE(pause);
    NATIVE_RESOLVE(kthread_add);
    NATIVE_RESOLVE(kthread_exit);
    NATIVE_RESOLVE(kthread_suspend_check);
    NATIVE_RESOLVE(kthread_set_affinity);

    /* Proc */
    NATIVE_RESOLVE(proc_kill);
    NATIVE_RESOLVE(proc_rwmem);
    NATIVE_RESOLVE(create_thread);

    /* ptrace */
    NATIVE_RESOLVE(kptrace);
    NATIVE_RESOLVE(kpsignal);
    NATIVE_RESOLVE(kwait);
    NATIVE_RESOLVE(kDelay);

    /* Virtual memory */
    NATIVE_RESOLVE(vmspace_acquire_ref);
    NATIVE_RESOLVE(vmspace_free);
    NATIVE_RESOLVE(vm_map_lock_read);
    NATIVE_RESOLVE(vm_map_unlock_read);
    NATIVE_RESOLVE(vm_map_lookup_entry);
    NATIVE_RESOLVE(vm_map_findspace);
    NATIVE_RESOLVE(vm_map_insert);
    NATIVE_RESOLVE(vm_map_lock);
    NATIVE_RESOLVE(vm_map_unlock);
    NATIVE_RESOLVE(vm_map_delete);
    NATIVE_RESOLVE(vm_map_protect);
    
    /*Mutex Locks*/
    NATIVE_RESOLVE(mtx_init);
    NATIVE_RESOLVE(mtx_destroy);
    NATIVE_RESOLVE(mtx_lock_flags);
    NATIVE_RESOLVE(mtx_unlock_flags);
    _mtx_lock_flags = decltype(_mtx_lock_flags)(mtx_lock_flags);
    _mtx_unlock_flags = decltype(_mtx_unlock_flags)(mtx_unlock_flags);

    /* Fake Selfs */
    NATIVE_RESOLVE(sceSblAuthMgrGetSelfInfo);
    NATIVE_RESOLVE(sceSblAuthMgrIsLoadable2);
    NATIVE_RESOLVE(sceSblAuthMgrSmStart);
    NATIVE_RESOLVE(sceSblAuthMgrVerifyHeader);
    NATIVE_RESOLVE(sceSblServiceMailbox);

    /* Critical Sections */
    NATIVE_RESOLVE(EnterCriticalSection);
    NATIVE_RESOLVE(ExitCriticalSection);

    /* Event Handling */
    NATIVE_RESOLVE(eventhandler_register);
    NATIVE_RESOLVE(eventhandler_deregister);
    NATIVE_RESOLVE(eventhandler_find_list);

    /* FileIO */
    NATIVE_RESOLVE(kern_open);
    NATIVE_RESOLVE(kern_mkdir);

    /* Kernel Misc */
    NATIVE_RESOLVE(kernel_sysctlbyname);
    NATIVE_RESOLVE(kernel_ioctl);

    /* Flash & NVS */
    NATIVE_RESOLVE(icc_nvs_read);
    NATIVE_RESOLVE(icc_nvs_write);

    /* Registry */
    NATIVE_RESOLVE(sceRegMgrGetStr);
    NATIVE_RESOLVE(sceRegMgrSetStr);
    NATIVE_RESOLVE(sceRegMgrGetInt);
    NATIVE_RESOLVE(sceRegMgrSetInt);
    NATIVE_RESOLVE(sceRegMgrGetBin);
    NATIVE_RESOLVE(sceRegMgrSetBin);

    return true;
}