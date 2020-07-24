#include "../Main.hpp"
#include "Network.hpp"

extern "C"
{
	#include <sys/proc.h>
    #include <sys/sysent.h>
    #include <sys/syscall.h>
}

int net_errno;

int sys_socket(int domain, int type, int protocol) 
{
	struct sys_socket_args {
		uint64_t domain;
		uint64_t type;
		uint64_t protocol;
	};

    auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

    auto sys_socket = (int(*)(thread * td, sys_socket_args * uap))sysents[SYS_socket].sy_call;

	thread *td = curthread();

    td->td_retval[0] = 0;

	struct sys_socket_args uap;
	uap.domain = domain;
	uap.type = type;
	uap.protocol = protocol;

	net_errno = sys_socket(td, &uap);

	return td->td_retval[0];
}