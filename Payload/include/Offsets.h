/* golden */
/* 1/2/2018 */

// inspired by fail0verflow, of course
// 5.05

// ref 0xFFFFFFFF87464000

#define __Xfast_syscall							0x1C0
#define __copyin								0x1EA710 
#define __copyout								0x1EA630
#define __printf								0x436040 
#define __malloc								0x10E250 
#define __free									0x10E460 
#define __memcpy								0x1EA530
#define __memset								0x3205C0
#define __memcmp								0x50AC0
#define __kmem_alloc							0xFCC80
#define __strlen                                0x3B71A0
#define __vsprintf                              0x436310
#define __sprintf                               0x436280

#define __disable_console_output                0x19ECEB0
#define __M_TEMP					        	0x14B4110
#define __kernel_map                            0x1AC60E0
#define __prison0                               0x10986A0
#define __rootvnode                             0x22C1A70

#define KERN_PMAP_PROTECT	                    0x2E3090
#define KERN_PMAP_PROTECT_P	                    0x2E30D4
#define KERN_PMAP_STORE		                    0x22CB570

#define DT_HASH_SEGMENT	                        0xB5EF30