#include "../main.hpp"
#include "Kernel.hpp"

extern "C"
{
	#include <sys/proc.h>
    #include <sys/sysent.h>
    #include <sys/syscall.h>
	#include "sys/sysproto.h"
}

int kern_errorno = 0;

caddr_t kmmap(caddr_t addr, size_t len, int prot, int flags, int fd, off_t pos) 
{
	struct sys_mmap_args {
		uint64_t addr;
		uint64_t len;
		uint64_t prot;
		uint64_t flags;
		uint64_t fd;
		uint64_t pos;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_mmap = (int(*)(thread * td, sys_mmap_args * uap))sysents[SYS_mmap].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_mmap_args uap;
	uap.addr = (uint64_t)addr;
	uap.len = len;
	uap.prot = prot;
	uap.flags = flags;
	uap.fd = fd;
	uap.pos = pos;

	kern_errorno = sys_mmap(td, &uap);

	return (caddr_t)td->td_retval[0];
}

int kmunmap(caddr_t addr, size_t len) 
{
	struct sys_munmap_args {
		uint64_t addr;
		uint64_t len;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_munmap = (int(*)(thread * td, sys_munmap_args* uap))sysents[SYS_munmap].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	sys_munmap_args uap;
	uap.addr = (uint64_t)addr;
	uap.len = len;

	kern_errorno = sys_munmap(td, &uap);

	return td->td_retval[0];
}

int kern_reboot(int opt) 
{
	struct reboot_args {
		uint64_t opt;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_reboot = (int(*)(thread * td, reboot_args* uap))sysents[37].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	reboot_args uap;
	uap.opt = (uint64_t)opt;

	kern_errorno = sys_reboot(td, &uap);

	return td->td_retval[0];
}

int kwait4(int pid, int *status, int options, struct rusage *rusage)
{
	struct kwait4_args {
		uint64_t pid;
		uint64_t status;
		uint64_t options;
		uint64_t rusage;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_kwait4 = (int(*)(thread * td, kwait4_args* uap))sysents[SYS_wait4].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	kwait4_args uap;
	uap.pid = pid;
	uap.status = (uint64_t)status;
	uap.options = options;
	uap.rusage = (uint64_t)rusage;

	kern_errorno = sys_kwait4(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	return td->td_retval[0];
}

int ksetuid(uid_t uid, thread* td)
{
	auto sv = (sysentvec*)resolve(addr_sysvec);
	sysent* sysents = sv->sv_table;
	auto sys_setuid = (int(*)(thread *, setuid_args *))sysents[23].sy_call;

	setuid_args uap;

	// clear errors
	td->td_retval[0] = 0;

	// call syscall
	uap.uid = uid;

	kern_errorno = sys_setuid(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	// success
	return td->td_retval[0];
}

int kdup2(int oldd, int newd, struct thread* td)
{
	auto sv = (sysentvec*)resolve(addr_sysvec);
 	sysent* sysents = sv->sv_table;
	auto sys_dup2 = (int(*)(thread *, dup2_args *))sysents[90].sy_call;

	struct dup2_args uap;

	// clear errors
	td->td_retval[0] = 0;

	// call syscall
	uap.from = oldd;
	uap.to = newd;

	kern_errorno = sys_dup2(td, &uap);
	if(kern_errorno)
		return -kern_errorno;

	// success
	return td->td_retval[0];
}