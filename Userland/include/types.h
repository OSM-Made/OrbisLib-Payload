#pragma once

#include <stdint.h>

#define NULL 0

#define FALSE 0
#define TRUE 1

#define PAGE_SIZE (16 * 1024)

#define BIT(n) (1 << (n))

typedef uint64_t size_t;
typedef int64_t ssize_t;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float float32;
typedef double float64;

typedef volatile uint8_t vuint8;
typedef volatile uint16_t vuint16;
typedef volatile uint32_t vuint32;
typedef volatile uint64_t vuint64;

typedef volatile int8_t vint8;
typedef volatile int16_t vint16;
typedef volatile int32_t vint32;
typedef volatile int64_t vint64;

typedef volatile float32 vfloat32;
typedef volatile float64 vfloat64;

typedef uint8_t byte;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

//typedef uint32_t __dev_t; 
typedef uint32_t blksize_t;
typedef int64_t  blkcnt_t;
typedef uint32_t dev_t;
typedef uint32_t fflags_t;
typedef uint32_t gid_t;
typedef uint32_t ino_t;
typedef uint16_t mode_t;
typedef uint16_t nlink_t;
typedef int64_t  off_t;
typedef uint32_t uid_t;
typedef int64_t time_t;
typedef long suseconds_t;

typedef unsigned int SceKernelUseconds;

typedef int vm_prot_t;
typedef uint64_t vm_offset_t;
typedef uint64_t vm_map_t;
typedef uint64_t vm_size_t;
typedef unsigned long size_t;
#define	VM_PROT_NONE	((vm_prot_t) 0x00)
#define VM_PROT_READ	((vm_prot_t) 0x01)	/* read permission */
#define VM_PROT_WRITE	((vm_prot_t) 0x02)	/* write permission */
#define VM_PROT_EXECUTE	((vm_prot_t) 0x04)	/* execute permission */
#define VM_PROT_DEFAULT	(VM_PROT_READ|VM_PROT_WRITE)
#define VM_PROT_ALL			(VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE)
#define VM_PROT_NO_CHANGE	((vm_prot_t) 0x08)
#define VM_PROT_COPY		((vm_prot_t) 0x10)
#define VM_PROT_WANTS_COPY	((vm_prot_t) 0x10)

#define JOIN_HELPER(x, y) x##y
#define JOIN(x, y) JOIN_HELPER(x, y)

#define TYPE_PAD(size) char JOIN(_pad_, __COUNTER__)[size]
#define TYPE_VARIADIC_BEGIN(name) name { union {
#define TYPE_BEGIN(name, size) name { union { TYPE_PAD(size)
#define TYPE_END(...) }; } __VA_ARGS__
#define TYPE_FIELD(field, offset) struct { TYPE_PAD(offset); field; }

#define TYPE_CHECK_SIZE(name, size) \
  _Static_assert(sizeof(name) == (size), "Size of " #name " != " #size)

#define TYPE_CHECK_FIELD_OFFSET(name, member, offset) \
  _Static_assert(offsetof(name, member) == (offset), "Offset of " #name "." #member " != " #offset)

#define TYPE_CHECK_FIELD_SIZE(name, member, size) \
  _Static_assert(sizeof(((name*)0)->member) == (size), "Size of " #name "." #member " != " #size)

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

struct lock_object {
	const char* lo_name;
	uint32_t lo_flags;
	uint32_t lo_data;
	void* lo_witness;
};

struct mtx {
	struct lock_object lock_object;
	volatile void* mtx_lock;
};

struct sx {
	struct lock_object lock_object;
	volatile uintptr_t sx_lock;
};

struct auditinfo_addr {
	uint8_t useless[184];
};

struct ucred {
	uint32_t cr_ref;					// reference count		0x0000
	uint32_t cr_uid;					// effective user id	0x0004
	uint32_t cr_ruid;					// real user id			0x0008
	uint32_t useless2;					// 						0x000C
	uint32_t useless3;					//
	uint32_t cr_rgid;					// real group id
	uint32_t useless4;					//
	void *useless5;						//
	void *useless6;						//
	void *cr_prison;					// jail(2)				0x0030
	void *useless7;						//
	uint32_t useless8;					//
	void *useless9[2];					//
	void *useless10;					//
	struct auditinfo_addr cr_audit;		//
	uint32_t *cr_groups;				// groups
	uint32_t useless12;					//
};

struct filedesc {
	void *useless1[3];
	void *fd_rdir;
	void *fd_jdir;
};

TYPE_BEGIN(struct proc, 0x800); // XXX: random, don't use directly without fixing it
TYPE_FIELD(struct proc *p_forw, 0);
TYPE_FIELD(struct ucred *p_ucred, 0x40);
TYPE_FIELD(struct filedesc *p_fd, 0x48);
TYPE_FIELD(int pid, 0xB0);
TYPE_FIELD(char p_comm[32], 0x44C);
TYPE_END();

TYPE_BEGIN(struct thread, 0x3D8); // XXX: random, don't use directly without fixing it
TYPE_FIELD(struct proc *td_proc, 8);
TYPE_FIELD(struct ucred *td_ucred, 0x130);
TYPE_FIELD(char td_name[32], 0x284);
TYPE_END();

#define CR0_WP (1 << 16) // write protect

static inline __attribute__((always_inline)) uint64_t __readmsr(uint32_t __register) {
	uint32_t __edx, __eax;

	__asm__ volatile (
	    "rdmsr"
	    : "=d"(__edx),
	    "=a"(__eax)
	    : "c"(__register)
	);

	return (((uint64_t)__edx) << 32) | (uint64_t)__eax;
}

static inline __attribute__((always_inline)) uint64_t __readcr0(void) {
	uint64_t cr0;

	__asm__ volatile (
	    "movq %0, %%cr0"
	    : "=r" (cr0)
	    : : "memory"
	);

	return cr0;
}
static inline __attribute__((always_inline)) void __writecr0(uint64_t cr0) {
	__asm__ volatile (
	    "movq %%cr0, %0"
	    : : "r" (cr0)
	    : "memory"
	);
}

#define AF_INET 0x0002

#define IN_ADDR_ANY 0

#define SOCK_STREAM 1
#define SOCK_DGRAM 2

#define SOL_SOCKET 0xffff
#define SO_NBIO 0x1200

/*
 * Option flags per-socket.
 */
#define	SO_DEBUG		0x0001		/* turn on debugging info recording */
#define	SO_ACCEPTCONN	0x0002		/* socket has had listen() */
#define	SO_REUSEADDR	0x0004		/* allow local address reuse */
#define	SO_KEEPALIVE	0x0008		/* keep connections alive */
#define	SO_DONTROUTE	0x0010		/* just use interface addresses */
#define	SO_BROADCAST	0x0020		/* permit sending of broadcast msgs */
#define	SO_USELOOPBACK	0x0040		/* bypass hardware when possible */
#define	SO_LINGER		0x0080		/* linger on close if data present */
#define	SO_OOBINLINE	0x0100		/* leave received OOB data in line */
#define	SO_REUSEPORT	0x0200		/* allow local address & port reuse */
#define	SO_TIMESTAMP	0x0400		/* timestamp received dgram traffic */
#define	SO_NOSIGPIPE	0x0800		/* no SIGPIPE from EPIPE */
#define	SO_ACCEPTFILTER	0x1000		/* there is an accept filter */
#define	SO_BINTIME		0x2000		/* timestamp received dgram traffic */
#define	SO_NO_OFFLOAD	0x4000		/* socket cannot be offloaded */
#define	SO_NO_DDP		0x8000		/* disable direct data placement */
#define SO_NBIO			0x1200

#define	SO_SNDBUF	0x1001		/* send buffer size */
#define	SO_RCVBUF	0x1002		/* receive buffer size */
#define	SO_SNDLOWAT	0x1003		/* send low-water mark */
#define	SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define	SO_SNDTIMEO	0x1005		/* send timeout */
#define	SO_RCVTIMEO	0x1006		/* receive timeout */
#define	SO_ERROR	0x1007		/* get error status and clear */
#define	SO_TYPE		0x1008		/* get socket type */

#define TCP_NODELAY     1       /* don't delay send to coalesce packets */
#define TCP_MAXSEG      2       /* set maximum segment size */
#define TCP_NOPUSH      4       /* don't push last block of write */
#define TCP_NOOPT       8       /* don't use TCP options */
#define TCP_MD5SIG      16      /* use MD5 digests (RFC2385) */
#define TCP_INFO        32      /* retrieve tcp_info structure */
#define TCP_CONGESTION  64      /* get/set congestion control algorithm */
#define TCP_CCALGOOPT   65      /* get/set cc algorithm specific options */
#define TCP_KEEPINIT    128     /* N, time to establish connection */
#define TCP_KEEPIDLE    256     /* L,N,X start keeplives after this period */
#define TCP_KEEPINTVL   512     /* L,N interval between keepalives */
#define TCP_KEEPCNT     1024    /* L,N number of keepalives before close */
#define TCP_FASTOPEN    1025    /* enable TFO / was created via TFO */
#define TCP_PCAP_OUT    2048    /* number of output packets to keep */
#define TCP_PCAP_IN     4096    /* number of input packets to keep */
#define TCP_FUNCTION_BLK 8192   /* Set the tcp function pointers to the specified stack */

#define MSG_DONTWAIT 0x80
#define MSG_WAITALL 0x40

#define IPPROTO_TCP 6
#define TCP_NODELAY 1

#define	INADDR_ANY			(uint32_t)0x00000000
#define	INADDR_BROADCAST	(uint32_t)0xffffffff	/* must be masked */

#define IP(a, b, c, d) (((a) << 0) + ((b) << 8) + ((c) << 16) + ((d) << 24))

enum {
	SCE_NET_IPPROTO_IP   = 0,
	SCE_NET_IPPROTO_ICMP = 1,
	SCE_NET_IPPROTO_IGMP = 2,
	SCE_NET_IPPROTO_TCP  = 6,
	SCE_NET_IPPROTO_UDP  = 17,
	SCE_NET_SOL_SOCKET   = 0xffff
};

enum {
	SCE_NET_SO_REUSEADDR = 0x00000004,
};

enum {
	SCE_NET_ERROR_EINTR = 0x80410104,
};

enum {
	SCE_NET_SOCKET_ABORT_FLAG_RCV_PRESERVATION	= 0x00000001,
	SCE_NET_SOCKET_ABORT_FLAG_SND_PRESERVATION	= 0x00000002
};

struct in_addr {
	unsigned int s_addr;
};

struct sockaddr_in {
	unsigned char sin_len;
	unsigned char sin_family;
	unsigned short sin_port;
	struct in_addr sin_addr;
	unsigned short sin_vport;
	char sin_zero[6];
};

struct sockaddr {
	unsigned char sin_len;
	unsigned char sa_family;
	char sa_data[14];
};