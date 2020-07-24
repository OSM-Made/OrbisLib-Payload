#pragma once

#ifdef SOFTWARE_VERSION_672

//Util
#define addr_Xfast_syscall		                0x1C0

//Patches

#define addr_sysvec                             0x01A8A398

//Network
#define addr_sys_socket 				        0x0
#define addr_sys_connect       		            0x0
#define addr_sys_bind        			        0x0
#define addr_sys_listen       			        0x0
#define addr_sys_accept       			        0x0
#define addr_sys_read        			        0x0
#define addr_sys_send                           0x0
#define addr_sys_setsockopt      		        0x0
#define addr_sys_close        			        0x0
#define addr_sys_netabort                       0x0

#endif