#pragma once

#include "../include/main.h"
#include "../include/types.h"

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
#define PAGE_SIZE 0x4000

typedef uint64_t vm_offset_t;
typedef uint64_t size_t;
typedef int64_t ssize_t;

#define	VM_PROT_NONE		0x00
#define VM_PROT_READ		0x01	/* read permission */
#define VM_PROT_WRITE		0x02	/* write permission */
#define VM_PROT_EXECUTE		0x04	/* execute permission */
#define VM_PROT_DEFAULT		(VM_PROT_READ | VM_PROT_WRITE)
#define VM_PROT_ALL			(VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE)
#define VM_PROT_NO_CHANGE	0x08
#define VM_PROT_COPY		0x10
#define VM_PROT_WANTS_COPY	0x10

#define PROT_READ	0x1     /* Page can be read.  */
#define PROT_WRITE	0x2     /* Page can be written.  */
#define PROT_EXEC	0x4     /* Page can be executed.  */
#define PROT_NONE	0x0     /* Page can not be accessed.  */

// errno
#define EPERM		1
#define ENOENT		2
#define ESRCH		3
#define EINTR		4
#define EIO			5
#define ENXIO		6
#define E2BIG		7
#define ENOEXEC		8
#define EBADF		9
#define ECHILD		10
#define EAGAIN		11
#define ENOMEM		12
#define EACCES		13
#define EFAULT		14
#define ENOTBLK		15
#define EBUSY		16
#define EEXIST		17
#define EXDEV		18
#define ENODEV		19
#define ENOTDIR		20
#define EISDIR		21
#define EINVAL		22
#define ENFILE		23
#define EMFILE		24
#define ENOTTY		25
#define ETXTBSY		26
#define EFBIG		27
#define ENOSPC		28
#define ESPIPE		29
#define EROFS		30
#define EMLINK		31
#define EPIPE		32
#define EDOM		33
#define ERANGE		34
#define ENOMSG		35
#define EIDRM		36
#define ECHRNG		37
#define EL2NSYNC	38
#define EL3HLT		39
#define EL3RST		40
#define ELNRNG		41
#define EUNATCH		42
#define ENOCSI		43
#define EL2HLT		44
#define EDEADLK		45
#define ENOLCK		46
#define ECANCELED	47
#define ENOTSUP		48
#define	ENETDOWN	50
#define	ENETUNREACH	51
#define	ENETRESET	52
#define	ECONNABORTED	53
#define	ECONNRESET	54
#define	ENOBUFS		55
#define	EISCONN		56
#define	ENOTCONN	57
#define	ESHUTDOWN	58
#define	ETOOMANYREFS	59
#define	ETIMEDOUT	60
#define	ECONNREFUSED	61

#define	TRACEBUF	struct qm_trace trace;

#define	TAILQ_FIRST(head) ((head)->tqh_first)
#define	TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)

#define	TAILQ_HEAD(name, type)									\
struct name {													\
	struct type *tqh_first;	/* first element */					\
	struct type **tqh_last;	/* addr of last next element */		\
	TRACEBUF													\
}

#define	TAILQ_ENTRY(type)											\
struct {															\
	struct type *tqe_next;	/* next element */						\
	struct type **tqe_prev;	/* address of previous next element */	\
	TRACEBUF														\
}

#define	LIST_ENTRY(type)											\
struct {															\
	struct type *le_next;	/* next element */						\
	struct type **le_prev;	/* address of previous next element */	\
}

#define	TAILQ_FOREACH(var, head, field)				\
	for ((var) = TAILQ_FIRST((head));				\
	    (var);										\
(var) = TAILQ_NEXT((var), field))

struct qm_trace {
	char * lastfile;
	int lastline;
	char * prevfile;
	int prevline;
};

struct reg {
	uint64_t r_r15;
	uint64_t r_r14;
	uint64_t r_r13;
	uint64_t r_r12;
	uint64_t r_r11;
	uint64_t r_r10;
	uint64_t r_r9;
	uint64_t r_r8;
	uint64_t r_rdi;
	uint64_t r_rsi;
	uint64_t r_rbp;
	uint64_t r_rbx;
	uint64_t r_rdx;
	uint64_t r_rcx;
	uint64_t r_rax;
	uint32_t r_trapno;
	uint16_t r_fs; // 0x7C
	uint16_t r_gs; // 0x7E
	uint32_t r_err;
	uint16_t r_es;
	uint16_t r_ds;
	uint64_t r_rip;
	uint64_t r_cs;
	uint64_t r_rflags;
	uint64_t r_rsp;
	uint64_t r_ss;
};

struct fpxreg
{
	uint8_t fp_bytes[10];
	uint8_t fp_pad[6];
};

struct xmmreg
{
	uint8_t element[16];
};

struct fpregs
{
	uint16_t en_cw; /* control word (16bits) */
	uint16_t en_sw; /* status word (16bits) */
	uint8_t en_tw; /* tag word (8bits) */
	uint8_t en_zero;
	uint16_t en_opcode; /* opcode last executed (11 bits ) */
	uint64_t en_rip; /* floating point instruction pointer */
	uint64_t en_rdp; /* floating operand pointer */
	uint32_t en_mxcsr; /* SSE sontorol/status register */
	uint32_t en_mxcsr_mask; /* valid bits in mxcsr */
	struct fpxreg _st[8];
	struct xmmreg _xmm[16];
	uint8_t padding0[0x1A0];
};

typedef uint32_t __dev_t;
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

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

struct timeval {
	uint64_t tv_sec;		/* seconds */
	uint64_t tv_usec;	/* and microseconds */
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

enum uio_rw {
	UIO_READ,
	UIO_WRITE
};

enum uio_seg {
	UIO_USERSPACE,		/* from user data space */
	UIO_SYSSPACE,		/* from system space */
	UIO_USERISPACE		/* from user I space */
};

struct iovec {
	uint64_t iov_base;
	size_t iov_len;
};

TYPE_BEGIN(struct uio, 0x30);
TYPE_FIELD(uint64_t uio_iov, 0);
TYPE_FIELD(uint32_t uio_iovcnt, 8);
TYPE_FIELD(uint64_t uio_offset, 0x10);
TYPE_FIELD(uint64_t uio_resid, 0x18);
TYPE_FIELD(uint32_t uio_segflg, 0x20);
TYPE_FIELD(uint32_t uio_rw, 0x24);
TYPE_FIELD(struct thread *uio_td, 0x28);
TYPE_END();

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

TYPE_BEGIN(struct vm_map_entry, 0xC0);
TYPE_FIELD(struct vm_map_entry *prev, 0);
TYPE_FIELD(struct vm_map_entry *next, 8);
TYPE_FIELD(struct vm_map_entry *left, 0x10);
TYPE_FIELD(struct vm_map_entry *right, 0x18);
TYPE_FIELD(vm_offset_t start, 0x20);
TYPE_FIELD(vm_offset_t end, 0x28);
TYPE_FIELD(vm_offset_t offset, 0x50);
TYPE_FIELD(uint16_t prot, 0x5C);
TYPE_FIELD(char name[32], 0x8D);
TYPE_END();

TYPE_BEGIN(struct vm_map, 0x178);
TYPE_FIELD(struct vm_map_entry header, 0);
TYPE_FIELD(struct sx lock, 0xB8);
TYPE_FIELD(struct mtx system_mtx, 0xD8);
TYPE_FIELD(int nentries, 0x100);
TYPE_END();

TYPE_BEGIN(struct vmspace, 0x250);
TYPE_FIELD(struct vm_map vm_map, 0);
// maybe I will add more later just for documentation purposes
TYPE_END();

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

/*
 * These were process status values (p_stat), now they are only used in
 * legacy conversion code.
 */
#define	SIDL	1		/* Process being created by fork. */
#define	SRUN	2		/* Currently runnable. */
#define	SSLEEP	3		/* Sleeping on an address. */
#define	SSTOP	4		/* Process debugging or suspension. */
#define	SZOMB	5		/* Awaiting collection by parent. */
#define	SWAIT	6		/* Waiting for interrupt. */
#define	SLOCK	7	/* Blocked on a lock. */

TYPE_BEGIN(struct proc_ldt , 0x100);
TYPE_FIELD(struct proc_ldt *md_ldt_next, 0x0);
TYPE_FIELD(const char* LibraryPath, 0x8);

TYPE_FIELD(int ModuleHandle, 0x28);

TYPE_FIELD(void *codeBase, 0x30);
TYPE_FIELD(unsigned int codeSize, 0x40);
TYPE_FIELD(void *dataBase, 0x48);
TYPE_FIELD(unsigned int dataSize, 0x50);

TYPE_END();

TYPE_BEGIN(struct mdproc , 0x10);
TYPE_FIELD(struct proc_ldt *md_ldt, 0x0);
TYPE_END();

TYPE_BEGIN(struct proc, 0x800); 
TYPE_FIELD(struct proc *p_forw, 0);
TYPE_FIELD(TAILQ_HEAD(, thread) p_threads, 0x10);
TYPE_FIELD(struct ucred *p_ucred, 0x40);
TYPE_FIELD(struct filedesc *p_fd, 0x48);
TYPE_FIELD(int flags, 0xA8);
TYPE_FIELD(char p_stat, 0xAC);
TYPE_FIELD(int pid, 0xB0);
TYPE_FIELD(struct vmspace *p_vmspace, 0x168);
TYPE_FIELD(struct mdproc *p_md, 0x340);
TYPE_FIELD(char titleId[10], 0x390);
TYPE_FIELD(char p_comm[32], 0x44C);
TYPE_END();


TYPE_BEGIN(struct thread, 0x3D8);
TYPE_FIELD(struct mtx *volatile td_lock, 0);
TYPE_FIELD(struct proc *td_proc, 8);
TYPE_FIELD(TAILQ_ENTRY(thread) td_plist, 0x10);
TYPE_FIELD(int tid, 0x88);
TYPE_FIELD(int td_inhibitors, 0xD8);
TYPE_FIELD(int td_pinned, 0x12C);
TYPE_FIELD(struct ucred *td_ucred, 0x130);
TYPE_FIELD(char td_name[32], 0x284);
TYPE_FIELD(uint64_t td_retval[2], 0x398);
TYPE_FIELD(uint16_t td_priority, 0x380);
TYPE_END();

static inline struct thread *curthread(void) {
	struct thread* td;
	__asm__ __volatile__ (
	    "mov %0, %%gs:0"
	    : "=r"(td)
	);

	return td;
}

struct moduleInfo {
    size_t size; // 0x0
    char name[0x100]; // 0x8
    void *codeBase; // 0x108
    unsigned int codeSize; // 0x110
    void *dataBase; // 0x118
    unsigned int dataSize; // 0x120
    char padding2[0x3C]; // 0x124
}; //0x160

#define NULL 0
#define offsetof(st, m) ((size_t)((char *)&((st *)(0))->m - (char *)0))
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define	PT_TRACE_ME	0	/* child declares it's being traced */
#define	PT_READ_I	1	/* read word in child's I space */
#define	PT_READ_D	2	/* read word in child's D space */
/* was	PT_READ_U	3	 * read word in child's user structure */
#define	PT_WRITE_I	4	/* write word in child's I space */
#define	PT_WRITE_D	5	/* write word in child's D space */
/* was	PT_WRITE_U	6	 * write word in child's user structure */
#define	PT_CONTINUE	7	/* continue the child */
#define	PT_KILL		8	/* kill the child process */
#define	PT_STEP		9	/* single step the child */
#define	PT_ATTACH	10	/* trace some running process */
#define	PT_DETACH	11	/* stop tracing a process */
#define PT_IO		12	/* do I/O to/from stopped process. */
#define	PT_LWPINFO	13	/* Info about the LWP that stopped. */
#define PT_GETNUMLWPS	14	/* get total number of threads */
#define PT_GETLWPLIST	15	/* get thread list */
#define PT_CLEARSTEP	16	/* turn off single step */
#define PT_SETSTEP	17	/* turn on single step */
#define PT_SUSPEND	18	/* suspend a thread */
#define PT_RESUME	19	/* resume a thread */
#define	PT_TO_SCE	20
#define	PT_TO_SCX	21
#define	PT_SYSCALL	22
#define	PT_FOLLOW_FORK	23
#define PT_GETREGS      33	/* get general-purpose registers */
#define PT_SETREGS      34	/* set general-purpose registers */
#define PT_GETFPREGS    35	/* get floating-point registers */
#define PT_SETFPREGS    36	/* set floating-point registers */
#define PT_GETDBREGS    37	/* get debugging registers */
#define PT_SETDBREGS    38	/* set debugging registers */
#define	PT_VM_TIMESTAMP	40	/* Get VM version (timestamp) */
#define	PT_VM_ENTRY	41	/* Get VM map (entry) */

#define __LOW(v)	((v) & 0377)
#define __HIGH(v)	(((v) >> 8) & 0377)

#define WNOHANG         1	/* do not wait for child to exit */
#define WUNTRACED       2	/* for job control; not implemented */
#define WCONTINUED      8 /* Report continued child.  */

#define WIFEXITED(s)	(__LOW(s) == 0)		    /* normal exit */
#define WEXITSTATUS(s)	(__HIGH(s))			    /* exit status */
#define WTERMSIG(s)	(__LOW(s) & 0177)		    /* sig value */
#define WIFSIGNALED(s)	((((unsigned int)(s)-1) & 0xFFFF) < 0xFF) /* signaled */
#define WIFSTOPPED(s) (__LOW(s) == 0177)		    /* stopped */
#define WSTOPSIG(s) (__HIGH(s) & 0377) /* stop signal */

#define	SIGHUP	1	/* hangup */
#define	SIGINT	2	/* interrupt */
#define	SIGQUIT	3	/* quit */
#define	SIGILL	4	/* illegal instruction (not reset when caught) */
#define	SIGTRAP	5	/* trace trap (not reset when caught) */
#define	SIGABRT	6	/* abort() */
#define	SIGIOT SIGABRT	/* compatibility */
#define	SIGEMT	7	/* EMT instruction */
#define	SIGFPE	8	/* floating point exception */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad argument to system call */
#define	SIGPIPE	13	/* write on a pipe with no one to read it */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* software termination signal from kill */
#define	SIGURG	16	/* urgent condition on IO channel */
#define	SIGSTOP	17	/* sendable stop signal not from tty */
#define	SIGTSTP	18	/* stop signal from tty */
#define	SIGCONT	19	/* continue a stopped process */
#define	SIGCHLD	20	/* to parent on child stop or exit */
#define	SIGTTIN	21	/* to readers pgrp upon background tty read */
#define	SIGTTOU	22	/* like TTIN for output if (tp->t_local&LTOSTOP) */
#define	SIGIO	23	/* input/output possible signal */
#define	SIGXCPU	24	/* exceeded CPU time limit */
#define	SIGXFSZ	25	/* exceeded file size limit */
#define	SIGVTALRM 26	/* virtual time alarm */
#define	SIGPROF	27	/* profiling time alarm */
#define SIGWINCH 28	/* window size changes */
#define SIGINFO	29	/* information request */
#define SIGUSR1 30	/* user defined signal 1 */
#define SIGUSR2 31	/* user defined signal 2 */

struct __dbreg64 {
	uint64_t	dr[16];	/* debug registers */
				/* Index 0-3: debug addresss registers */
				/* Index 4-5: reserved */
				/* Index 6: debug status */
				/* Index 7: debug control */
				/* Index 8-15: reserved */
};

#define	DBREG_DR7_LOCAL_ENABLE	0x01
#define	DBREG_DR7_GLOBAL_ENABLE	0x02
#define	DBREG_DR7_LEN_1		0x00	/* 1 byte length          */
#define	DBREG_DR7_LEN_2		0x01
#define	DBREG_DR7_LEN_4		0x03
#define	DBREG_DR7_LEN_8		0x02
#define	DBREG_DR7_EXEC		0x00	/* break on execute       */
#define	DBREG_DR7_WRONLY	0x01	/* break on write         */
#define	DBREG_DR7_RDWR		0x03	/* break on read or write */
#define	DBREG_DR7_MASK(i)	((uint64_t)(0xf) << ((i) * 4 + 16) | 0x3 << (i) * 2)
#define	DBREG_DR7_SET(i, len, access, enable)	((uint64_t)((len) << 2 | (access)) << ((i) * 4 + 16) | (enable) << (i) * 2)
#define	DBREG_DR7_GD		0x2000
#define	DBREG_DR7_ENABLED(d, i)	(((d) & 0x3 << (i) * 2) != 0)
#define	DBREG_DR7_ACCESS(d, i)	((d) >> ((i) * 4 + 16) & 0x3)
#define	DBREG_DR7_LEN(d, i)	((d) >> ((i) * 4 + 18) & 0x3)

#define	DBREG_DRX(d,x)	((d)->dr[(x)])	/* reference dr0 - dr7 by register number */

#define MAX_WATCHPOINTS 4
