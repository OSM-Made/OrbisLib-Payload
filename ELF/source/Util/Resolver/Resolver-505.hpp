#pragma once

#ifdef SOFTWARE_VERSION_505

/* Util */
#define addr_Xfast_syscall		                0x1C0
#define addr_sysvec                             0x19BBCD0
#define addr_kernel_map                         0x1AC60E0
#define addr_prison0                            0x10986A0
#define addr_rootvnode                          0x22C1A70
#define addr_copyin								0x1EA710
#define addr_copyout							0x1EA630

/* Patches */
#define addr_copyin1                            0x1EA758
#define addr_copyin2                            0x1EA767
#define addr_copyout1                           0x1EA66B
#define addr_copyout2                           0x1EA673
#define addr_copyout3                           0x1EA682

/* STD Lib */
#define addr_M_TEMP                             0x14B4110
#define addr_malloc								0x10E250 
#define addr_free					    		0x10E460 
#define addr_memcpy								0x1EA530 
#define addr_memset								0x3205C0
#define addr_memcmp								0x50AC0 
#define addr_strlen                             0x3B71A0
#define addr_strcpy								0x8F250
#define addr_strcmp                             0x1D0FD0 
#define addr_sprintf                            0x436280 
#define addr_vsprintf                           0x436310 
#define addr_vprintf                            0x4360B0 

#define addr_kmem_alloc							0xFCC80

/* Kproc */
#define addr_kproc_create					    0x137DF0
#define addr_kproc_exit                         0x138060
#define addr_kproc_resume                       0x1381B0
#define addr_kproc_shutdown                     0x10DCA0
#define addr_kproc_start                        0x137D70
#define addr_kproc_suspend                      0x138110
#define addr_kproc_suspend_check                0x138240
#define addr_kproc_kthread_add				    0x138B70
#define addr_pause                              0x3FB920
#define addr_kthread_add						0x138360
#define addr_kthread_exit						0x138640
#define addr_kthread_suspend					0x0
#define addr_kthread_suspend_check				0x138A60
#define addr_kthread_set_affinity				0x138CC0

/* Proc */
#define addr_allproc						    0x2382FF8
#define addr_proc_kill                          0xD41C0
#define addr_proc_rwmem							0x30D150

/* ptrace */
#define addr_kptrace                            0x30D8E0
#define addr_kpsignal                           0xD35F0
#define addr_kwait                              0x35590
#define addr_kDelay                             0x2A1C70

/* Virtual Memory */
#define addr_vmspace_acquire_ref				0x19EF90
#define addr_vmspace_free						0x19EDC0
#define addr_vm_map_lock_read					0x19F140
#define addr_vm_map_unlock_read					0x19F190
#define addr_vm_map_lookup_entry				0x19F760
#define addr_vm_map_findspace					0x1A1F60
#define addr_vm_map_insert						0x1A0280
#define addr_vm_map_lock						0x19EFF0
#define addr_vm_map_unlock 						0x19F060
#define addr_vm_map_delete						0x1A19D0
#define addr_vm_map_protect						0x1A3A50

#endif