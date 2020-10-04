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
int (*sprintf)(char* dst, const char *fmt, ...);
int (*snprintf)(char *str, size_t size, const char *format, ...);
int (*vsprintf)(char* dst, const char* fmt, va_list ap);
int (*vprintf)(const char *fmt, va_list arg);
int(*sscanf)(const char *str, const char *format, ...);
char *(*strdup)(const char *s, void* type);
char *(*realloc)(void *addr, unsigned long size, void* mtp, int flags);

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
#ifdef SOFTWARE_VERSION_672 //5.5X -> 6.72
eventhandler_tag (*eventhandler_register)(eventhandler_list *list, const char *name, void *func, const char* unk, void *arg, int priority); //6.72
#endif
void (*eventhandler_deregister)(eventhandler_list* a, eventhandler_entry* b);
eventhandler_list* (*eventhandler_find_list)(const char *name);

/* FileIO */
int (*kern_open)(thread* td, char *path, int pathseg, int flags, int mode);
int (*kern_mkdir)(thread* td, char *path, int pathseg, int mode);

/* Kernel Misc */ 
int (*kernel_sysctlbyname)(thread *td, char *name, void *old, size_t *oldlenp, void *pnew, size_t newlen, size_t *retval, int flags);
int (*kernel_ioctl)(thread *td, int fd, unsigned long request, ...);

/* Registry */
int (*sceRegMgrGetStr)(uint64_t RegID, char* Value, int len);
int (*sceRegMgrSetStr)(uint64_t RegID, char* Value, int len);
int (*sceRegMgrGetInt)(uint64_t RegID, int32_t* Value);
int (*sceRegMgrSetInt)(uint64_t RegID, int32_t Value);
int (*sceRegMgrGetBin)(uint64_t RegID, char* Value, int size);
int (*sceRegMgrSetBin)(uint64_t RegID, char* Value, int size);

void ResolveFunctions()
{
    //something = ()resolve();

    /* STD Lib */
    M_TEMP = resolve(addr_M_TEMP);
    M_MOUNT = resolve(addr_M_MOUNT);
    malloc = (void*(*)(unsigned long size, void *type, int flags))resolve(addr_malloc);
    free = (void(*)(void *addr, void *type))resolve(addr_free);
    memcpy = (void(*)(void *dst, const void *src, size_t len))resolve(addr_memcpy);
    memset = (void*(*)(void *ptr, int value, size_t num))resolve(addr_memset);
    memcmp = (int(*)(const void *ptr1, const void *ptr2, size_t num))resolve(addr_memcmp);
    strlen = (size_t(*)(const char *str))resolve(addr_strlen);
    strcpy = (int(*)(char * str1, char * str2))resolve(addr_strcpy);
    strncpy = (char*(*)(char *destination, const char *source, size_t num))resolve(addr_strncpy);
    strcmp = (int(*)(const char * str1, const char * str2))resolve(addr_strcmp);
    sprintf = (int(*)(char* dst, const char *fmt, ...))resolve(addr_sprintf);
    snprintf = (int(*)(char *str, size_t size, const char *format, ...))resolve(addr_snprintf);
    vsprintf = (int(*)(char* dst, const char* fmt, va_list ap))resolve(addr_vsprintf);
    vprintf = (int(*)(const char *fmt, va_list arg))resolve(addr_vprintf);
    sscanf = (int(*)(const char *str, const char *format, ...))resolve(addr_sscanf);
    strdup = (char*(*)(const char *s, void*))resolve(addr_strdup);
    realloc = (char*(*)(void *addr, unsigned long size, void* mtp, int flags))resolve(addr_realloc);

    /* Kproc */
    kproc_create = (int(*)(void (*func)(void *), void *arg, struct proc **newpp, int flags, int pages, const char *fmt, ...))resolve(addr_kproc_create);
    kproc_exit = (int(*)(int code))resolve(addr_kproc_exit);
    //kproc_resume
    //kproc_shutdown
    //kproc_start
    //kproc_suspend
    //kproc_suspend_check
    kproc_kthread_add = (int(*)(void (*func)(void *), void *arg, struct proc **procptr, struct thread **tdptr, int flags, int pages, char * procname, const char *fmt, ...))resolve(addr_kproc_kthread_add);
    pause = (void(*)(const char *wmesg, int timo))resolve(addr_pause);
    kthread_add = (int(*)(void (*func)(void *), void *arg, struct proc *procp, struct thread **newtdpp, int flags, int pages, const char *fmt, ...))resolve(addr_kthread_add);
    kthread_exit = (void(*)(void))resolve(addr_kthread_exit);
    //kthread_suspend
    kthread_suspend_check = (void(*)(void))resolve(addr_kthread_suspend_check);
    kthread_set_affinity = (void(*)(const char *tdname, uint64_t prio, uint64_t cpuset, uint64_t unknown))resolve(addr_kthread_set_affinity);

    /* Proc */
    proc_kill = (int(*)(proc *p, char* why))resolve(addr_proc_kill);
    proc_rwmem = (int(*)(proc *p, uio *uio))resolve(addr_proc_rwmem);
    create_thread = (int(*)(thread * td, uint64_t ctx, void* start_func, void *arg, char *stack_base, size_t stack_size, char *tls_base, long * child_tid, long * parent_tid, uint64_t flags, uint64_t rtp))resolve(addr_create_thread);

    /* ptrace */
    kptrace = (int(*)(thread * td, int req, int pid, void * addr, int data))resolve(addr_kptrace);
    kpsignal = (int(*)(proc* proc, int sig))resolve(addr_kpsignal);
    kwait = (int(*)(thread *td, int wpid, int *status, int options, void *rusage))resolve(addr_kwait);
    kDelay = (int(*)(uint64_t time))resolve(addr_kDelay);

    /* Virtual memory */
    vmspace_acquire_ref = (vmspace*(*)(proc *p))resolve(addr_vmspace_acquire_ref);
    vmspace_free = (void(*)(vmspace *vm))resolve(addr_vmspace_free);
    vm_map_lock_read = (void(*)(vm_map* map))resolve(addr_vm_map_lock_read);
    vm_map_unlock_read = (void(*)(vm_map* map))resolve(addr_vm_map_unlock_read);
    vm_map_lookup_entry = (int(*)(vm_map* map, uint64_t address, vm_map_entry **entries))resolve(addr_vm_map_lookup_entry);
    vm_map_findspace = (int(*)(vm_map* map, uint64_t start, uint64_t length, uint64_t *addr))resolve(addr_vm_map_findspace);
    vm_map_insert = (int(*)(vm_map* map, uint64_t object, uint64_t offset, uint64_t start, uint64_t end, int prot, int max, int cow))resolve(addr_vm_map_insert);
    vm_map_lock = (void(*)(vm_map* map))resolve(addr_vm_map_lock);
    vm_map_unlock = (void(*)(vm_map* map))resolve(addr_vm_map_unlock);
    vm_map_delete = (int(*)(vm_map* map, uint64_t start, uint64_t end))resolve(addr_vm_map_delete);
    vm_map_protect = (int(*)(vm_map* map, uint64_t start, uint64_t end, int new_prot, uint64_t set_max))resolve(addr_vm_map_protect);
    
    /*Mutex Locks*/
    mtx_init = (void(*)(mtx *m, const char *name, const char *type, int opts))resolve(addr_mtx_init);
    mtx_destroy = (void(*)(mtx *mutex))resolve(addr_mtx_destroy);
    mtx_lock_flags = (void(*)(mtx *mutex, int flags))resolve(addr_mtx_lock_flags);
    mtx_unlock_flags = (void(*)(mtx *mutex, int flags))resolve(addr_mtx_unlock_flags);
    _mtx_lock_flags = (void(*)(mtx *mutex, int flags, const char *file, int line))resolve(addr_mtx_lock_flags);
    _mtx_unlock_flags = (void(*)(mtx *mutex, int flags, const char *file, int line))resolve(addr_mtx_unlock_flags);

    /* Fake Selfs */
    sceSblAuthMgrGetSelfInfo = (int(*)(SelfContext* ctx, void *exInfo))resolve(addr_sceSblAuthMgrGetSelfInfo);
    sceSblAuthMgrIsLoadable2 = (int(*)(SelfContext* pSelfContext, SelfAuthInfo* pOldAuthInfo, int32_t pPathId, SelfAuthInfo* pNewAuthInfo))resolve(addr_sceSblAuthMgrIsLoadable2);
    sceSblAuthMgrSmStart = (void(*)(void**))resolve(addr_sceSblAuthMgrSmStart);
    sceSblAuthMgrVerifyHeader = (int(*)(SelfContext* pSelfContext))resolve(addr_sceSblAuthMgrVerifyHeader);
    sceSblServiceMailbox = (int(*)(uint32_t pServiceId, void* pRequest, void* pResponse))resolve(addr_sceSblServiceMailbox);

    /* Critical Sections */
    EnterCriticalSection = (void(*)())resolve(addr_EnterCriticalSection);
    ExitCriticalSection = (void(*)())resolve(addr_ExitCriticalSection);

    /* Event Handling */
    #ifdef SOFTWARE_VERSION_505
    eventhandler_register = (eventhandler_tag(*)(eventhandler_list *list, const char *name, void *func, void *arg, int priority))resolve(addr_eventhandler_register);
    #endif
    #ifdef SOFTWARE_VERSION_672
    eventhandler_register = (eventhandler_tag(*)(eventhandler_list *list, const char *name, void *func, const char* unk, void *arg, int priority))resolve(addr_eventhandler_register);
    #endif
    eventhandler_deregister = (void(*)(struct eventhandler_list* a, struct eventhandler_entry* b))resolve(addr_eventhandler_deregister);
    eventhandler_find_list = (eventhandler_list * (*)(const char *name))resolve(addr_eventhandler_find_list);

    /* FileIO */
    kern_open = (int(*)(thread* td, char *path, int pathseg, int flags, int mode))resolve(addr_kern_open);
    kern_mkdir = (int(*)(thread* td, char *path, int pathseg, int mode))resolve(addr_kern_mkdir);

    /* Kernel Misc */
    kernel_sysctlbyname = (int(*)(thread *td, char *name, void *old, size_t *oldlenp, void *pnew, size_t newlen, size_t *retval, int flags))resolve(addr_kernel_sysctlbyname);
    kernel_ioctl = (int(*)(thread *td, int fd, unsigned long request, ...))resolve(addr_kernel_ioctl);

    /* Registry */
    sceRegMgrGetStr = (int(*)(uint64_t RegID, char* Value, int len))resolve(addr_sceRegMgrGetStr);
    sceRegMgrSetStr = (int(*)(uint64_t RegID, char* Value, int len))resolve(addr_sceRegMgrSetStr);
    sceRegMgrGetInt = (int(*)(uint64_t RegID, int32_t* Value))resolve(addr_sceRegMgrGetInt);
    sceRegMgrSetInt = (int(*)(uint64_t RegID, int32_t Value))resolve(addr_sceRegMgrSetInt);
    sceRegMgrGetBin = (int(*)(uint64_t RegID, char* Value, int size))resolve(addr_sceRegMgrGetBin);
    sceRegMgrSetBin = (int(*)(uint64_t RegID, char* Value, int size))resolve(addr_sceRegMgrSetBin);
}