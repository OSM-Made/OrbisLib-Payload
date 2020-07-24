#pragma once

#ifdef SOFTWARE_VERSION_505

//Util
#define addr_Xfast_syscall		                0x1C0

//Patches


#define addr_sysvec                             0x019BBCD0

//Network
#define addr_sys_socket 				        0x318EE0
#define addr_sys_connect       		            0x31A310
#define addr_sys_bind        			        0x319820
#define addr_sys_listen       			        0x319A60
#define addr_sys_accept       			        0x31A170
#define addr_sys_read        			        0x152AB0
#define addr_sys_send                           0x152FC0
#define addr_sys_setsockopt      		        0x31B750
#define addr_sys_close        			        0xC0EB0
#define addr_sys_netabort                       0x319200

//File IO

#endif