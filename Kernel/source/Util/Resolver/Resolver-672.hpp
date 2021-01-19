#pragma once

#ifdef SOFTWARE_VERSION_672

//Util
#define addr_Xfast_syscall		                0x1C0
#define addr_sysvec                             0x1A8A398
#define addr_kernel_map                         0x220DFC0
#define addr_prison0                            0x113E518
#define addr_rootvnode                          0x2300320
#define addr_copyin								0x3C17A0
#define addr_copyout							0x3C16B0

/* Patches */
#define patch_copyin1                           0x3C17E8
#define patch_copyin2                           0x3C17F7
#define patch_copyout1                          0x3C16EB
#define patch_copyout2                          0x3C16F3
#define patch_copyout3                          0x3C1702
#define patch_swword_lwpid_0                    0x3C1AC2
#define patch_swword_lwpid_1                    0x3C1AD1
#define patch_mount                             0x44026A

/* STD Lib */
#define addr_M_TEMP                             0x1540EB0
#define addr_M_MOUNT                            0x1A90CA0
#define addr_malloc								0xD7A0
#define addr_free					    		0xD9A0
#define addr_memcpy								0x3C15B0
#define addr_memset								0x1687D0
#define addr_memcmp								0x207E40
#define addr_strlen                             0x2433E0
#define addr_strcpy								0x2390C0
#define addr_strncpy                            0x329010
#define addr_strcmp                             0x341810
#define addr_strstr                             0x4817F0
#define addr_sprintf                            0x1234C0
#define addr_snprintf                           0x123590
#define addr_vsprintf                           0x123550
#define addr_vprintf                            0x1232F0
#define addr_sscanf                             0x243810
#define addr_strdup                             0x2504C0
#define addr_realloc                            0xDAD0

#define addr_kmem_alloc							0x250730

/* Kproc */
#define addr_kproc_create					    0x8A0A0
#define addr_kproc_exit                         0x8A310
#define addr_kproc_resume                       0x8A460
#define addr_kproc_shutdown                     0x207670
#define addr_kproc_start                        0x8A020
#define addr_kproc_suspend                      0x8A3C0
#define addr_kproc_suspend_check                0x8A4F0
#define addr_kproc_kthread_add				    0x8AE20
#define addr_pause                              0x22A080
#define addr_kthread_add						0x8A600
#define addr_kthread_exit						0x8A8F0
#define addr_kthread_suspend					0x8AA40
#define addr_kthread_suspend_check				0x8AD10
#define addr_kthread_set_affinity				0x8AF70

/* Proc */
#define addr_allproc						    0x22BBE80
#define addr_proc_kill                          0x2DC80
#define addr_proc_rwmem							0x10EE10
#define addr_create_thread                      0x4A6FB0

/* ptrace */
#define addr_kptrace                            0x10F7A0
#define addr_kpsignal                           0x1CF510
#define addr_kwait                              0x406950
#define addr_kDelay                             0x2F9DE0

/* Virtual Memory */
#define addr_vmspace_acquire_ref				0x44CB90
#define addr_vmspace_free						0x44C9C0
#define addr_vm_map_lock_read					0x44CD40
#define addr_vm_map_unlock_read					0x44CD90
#define addr_vm_map_lookup_entry				0x44D330
#define addr_vm_map_findspace					0x44FE60
#define addr_vm_map_insert						0x44DEF0
#define addr_vm_map_lock						0x44CBF0
#define addr_vm_map_unlock 						0x44CC60
#define addr_vm_map_delete						0x44F8A0
#define addr_vm_map_protect						0x451BF0

/*Mutex Locks*/
#define addr_mtx_init                           0x00496FE0
#define addr_mtx_destroy                        0x00497050
#define addr_mtx_lock_flags                     0x00496540
#define addr_mtx_unlock_flags                   0x00496810

/* Fake Selfs */
#define addr_sbl_drv_msg_mtx                    0x266AC70
#define addr_gpu_va_page_list                   0x266AC68
#define addr_mini_syscore_self_binary           0x0156A588
#define addr_sceSblAuthMgrGetSelfInfo           0x65E010
#define addr_sceSblAuthMgrIsLoadable2           0x65D7A0
#define addr_sceSblAuthMgrSmStart               0x65E490
#define addr_sceSblAuthMgrVerifyHeader          0x65D800
#define addr_sceSblServiceMailbox               0x64CC20
#define addr_sceSblAuthMgrVerifyHeaderHook1     0x659AC6
#define addr_sceSblAuthMgrVerifyHeaderHook2     0x65A758
#define addr_SceSblAuthMgrIsLoadable2Hook       0x65930F
#define addr_SceSblAuthMgrSmLoadSelfSegment_Mailbox 0x66092A
#define addr_SceSblAuthMgrSmLoadSelfBlock_Mailbox 0x661571

/* Critical Sections */
#define addr_EnterCriticalSection               0x2AA0A0
#define addr_ExitCriticalSection                0x2AA0B0

/* Event Handling */
#define addr_eventhandler_register              0x402E80
#define addr_eventhandler_deregister            0x403220
#define addr_eventhandler_find_list             0x403420

/* Trap Hooks */
#define addr_trapHook                           0
#define addr_trap_fatalHook                     0x2ED2E0

/* FileIO */
#define addr_kern_open                          0x49E990
#define addr_kern_mkdir                         0x4A3A80

/* Kernel Misc */
#define addr_kernel_sysctlbyname                0x2782E0
#define addr_kernel_ioctl                       0x39C5C0
#define addr_icc_indicator_set_buzzer           0x9A250

/* Registry */
#define addr_sceRegMgrGetStr                    0x509220
#define addr_sceRegMgrSetStr                    0x509060
#define addr_sceRegMgrGetInt                    0x508A60
#define addr_sceRegMgrSetInt                    0x5077D0
#define addr_sceRegMgrGetBin                    0x5093A0
#define addr_sceRegMgrSetBin                    0x5092F0

/* Flash & NVS */
#define addr_icc_nvs_read                       0x464450
#define addr_icc_nvs_write                      0x464290

/* Module Offsets */
#define addr_thr_initial_libkernel              0x435420
#define addr_thr_initial_libkernel_web          0x435420
#define addr_thr_initial_libkernel_sys          0x435830
#define addr_thr_initial_minisyscore            0

#endif