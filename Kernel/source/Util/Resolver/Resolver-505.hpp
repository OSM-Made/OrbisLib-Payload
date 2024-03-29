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
#define patch_copyin1                           0x1EA758
#define patch_copyin2                           0x1EA767
#define patch_copyout1                          0x1EA66B
#define patch_copyout2                          0x1EA673
#define patch_copyout3                          0x1EA682
#define patch_swword_lwpid_0                    0x1EA9D2
#define patch_swword_lwpid_1                    0x1EA9E1
#define patch_mount                             0x1DEBFE

/* STD Lib */
#define addr_M_TEMP                             0x14B4110
#define addr_M_MOUNT                            0x19BF300
#define addr_malloc								0x10E250 
#define addr_free					    		0x10E460 
#define addr_memcpy								0x1EA530 
#define addr_memset								0x3205C0
#define addr_memcmp								0x50AC0 
#define addr_strlen                             0x3B71A0
#define addr_strcpy								0x8F250
#define addr_strncpy                            0x3C0B0
#define addr_strcmp                             0x1D0FD0 
#define addr_strstr                             0x17DFB0
#define addr_sprintf                            0x436280
#define addr_snprintf                           0x436350
#define addr_vsprintf                           0x436310 
#define addr_vprintf                            0x4360B0 
#define addr_sscanf                             0x175900
#define addr_strdup                             0x1C1C30
#define addr_realloc                            0x10E590
#define addr_kprintf                            0x436040

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
#define addr_create_thread                      0x1BE1F0

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

/*Mutex Locks*/
#define addr_mtx_init                           0x402780
#define addr_mtx_destroy                        0x4027F0
#define addr_mtx_lock_flags                     0x401CD0
#define addr_mtx_unlock_flags                   0x401FA0

/* Fake Selfs */
#define addr_sbl_drv_msg_mtx                    0x271E210
#define addr_gpu_va_page_list                   0x271E208
#define addr_mini_syscore_self_binary           0x14C9D48
#define addr_sceSblAuthMgrGetSelfInfo           0x63CD40
#define addr_sceSblAuthMgrIsLoadable2           0x63C4F0
#define addr_sceSblAuthMgrSmStart               0x6418E0
#define addr_sceSblAuthMgrVerifyHeader          0x63C550
#define addr_sceSblServiceMailbox               0x632540
#define addr_sceSblAuthMgrVerifyHeaderHook1     0x63EAFC
#define addr_sceSblAuthMgrVerifyHeaderHook2     0x63F718
#define addr_SceSblAuthMgrIsLoadable2Hook       0x63E3A1
#define addr_SceSblAuthMgrSmLoadSelfSegment_Mailbox 0x64318B
#define addr_SceSblAuthMgrSmLoadSelfBlock_Mailbox 0x643DA2
#define addr_sceSblACMgrGetPathId               0x117E0
#define addr_sceSblAuthMgrIsLoadable__sceSblACMgrGetPathId 0x63E25D

/* Critical Sections */
#define addr_EnterCriticalSection               0x28E7A0
#define addr_ExitCriticalSection                0x28E7B0

/* Event Handling */
#define addr_eventhandler_register              0x1EC400
#define addr_eventhandler_deregister            0x1EC790
#define addr_eventhandler_find_list             0x1EC980

/* Trap Hooks */
#define addr_trapHook                           0x170E80
#define addr_trap_fatalHook                     0x171580

/* FileIO */
#define addr_kern_open                          0x33B9B0
#define addr_kern_mkdir                         0x340B70

/* Kernel Misc */
#define addr_kernel_sysctlbyname                0x262770
#define addr_kernel_ioctl                       0x153990
#define addr_icc_indicator_set_buzzer           0x1042E0

/* Registry Functions */
#define addr_sceRegMgrGetStr                    0x4FA550
#define addr_sceRegMgrSetStr                    0x4FA390
#define addr_sceRegMgrGetInt                    0x4F9E50
#define addr_sceRegMgrSetInt                    0x4F8D10
#define addr_sceRegMgrGetBin                    0x4FA6D0
#define addr_sceRegMgrSetBin                    0x4FA620

/* Flash & NVS */
#define addr_icc_nvs_read                       0x395830
#define addr_icc_nvs_write                      0x395670

/* Module Offsets */
#define addr_thr_initial_libkernel              0x84C20
#define addr_thr_initial_libkernel_web          0x84C20
#define addr_thr_initial_libkernel_sys          0x89030

#endif