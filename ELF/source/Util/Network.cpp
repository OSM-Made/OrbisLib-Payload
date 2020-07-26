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

	sys_socket_args uap;
	uap.domain = domain;
	uap.type = type;
	uap.protocol = protocol;

	net_errno = sys_socket(td, &uap);

	return td->td_retval[0];
}

int sys_connect(int sockfd, struct sockaddr *addr, int addrlen) 
{
	struct sys_connect_args {
		uint64_t sockfd;
		uint64_t name;
		uint64_t namelen;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

	auto sys_connect = (int(*)(thread * td, sys_connect_args * uap))sysents[SYS_connect].sy_call;

	thread *td = curthread();
	td->td_retval[0] = 0;

	sys_connect_args uap;
	uap.sockfd = sockfd;
	uap.name = (uint64_t)addr;
	uap.namelen = addrlen;

	net_errno = sys_connect(td, &uap);

	return net_errno;
}

int sys_bind(int sockfd, struct sockaddr *addr, int addrlen) 
{
	struct sys_bind_args {
		uint64_t sockfd;
		uint64_t name;
		uint64_t namelen;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

	auto sys_bind = (int(*)(thread * td, sys_bind_args * uap))sysents[SYS_bind].sy_call;

	thread *td = curthread();
	td->td_retval[0] = 0;

	sys_bind_args uap;
	uap.sockfd = sockfd;
	uap.name = (uint64_t)addr;
	uap.namelen = addrlen;

	net_errno = sys_bind(td, &uap);

	return net_errno;
}

int sys_listen(int sockfd, int backlog) 
{
	struct sys_listen_args {
		uint64_t sockfd;
		uint64_t backlog;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

	auto sys_listen = (int(*)(thread * td, sys_listen_args * uap))sysents[SYS_listen].sy_call;

	thread *td = curthread();
	td->td_retval[0] = 0;

	sys_listen_args uap;
	uap.sockfd = sockfd;
	uap.backlog = backlog;

	net_errno = sys_listen(td, &uap);

	return net_errno;
}

int sys_accept(int sockfd, struct sockaddr *addr, int *addrlen) 
{
	struct sys_accept_args {
		uint64_t sockfd;
		uint64_t name;
		uint64_t namelen;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

	auto sys_accept = (int(*)(thread * td, sys_accept_args * uap))sysents[SYS_accept].sy_call;

	thread *td = curthread();
	td->td_retval[0] = 0;

	sys_accept_args uap;
	uap.sockfd = sockfd;
	uap.name = (uint64_t)addr;
	uap.namelen = (uint64_t)addrlen;

	net_errno = sys_accept(td, &uap);

	return td->td_retval[0];
}

int sys_read(int fd, void *buf, uint64_t len) 
{
	struct sys_read_args {
		uint64_t fd;
		uint64_t buf;
		uint64_t nbyte;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

	auto sys_read = (int(*)(thread * td, sys_read_args * uap))sysents[SYS_read].sy_call;

	thread *td = curthread();
	td->td_retval[0] = 0;

	sys_read_args uap;
	uap.fd = fd;
	uap.buf = (uint64_t)buf;
	uap.nbyte = len;

	net_errno = sys_read(td, &uap);

	return td->td_retval[0];
}

int sys_write(int fd, const void *buf, uint64_t len) 
{
	struct sys_write_args {
		uint64_t fd;
		uint64_t buf;
		uint64_t nbyte;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

	auto sys_write = (int(*)(thread * td, sys_write_args * uap))sysents[SYS_write].sy_call;

	thread *td = curthread();
	td->td_retval[0] = 0;

	sys_write_args uap;
	uap.fd = fd;
	uap.buf = (uint64_t)buf;
	uap.nbyte = len;

	net_errno = sys_write(td, &uap);

	return td->td_retval[0];
}

int sys_setsockopt(int s, int level, int optname, const void *optval, uint32_t optlen) 
{
	struct sys_setsockopt_args {
		uint64_t fd;
		uint64_t level;
		uint64_t optname;
		uint64_t optval;
		uint64_t optlen;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

	auto sys_setsockopt = (int(*)(thread * td, sys_setsockopt_args * uap))sysents[SYS_setsockopt].sy_call;

	thread *td = curthread();
	td->td_retval[0] = 0;

	sys_setsockopt_args uap;
	uap.fd = s;
	uap.level = level;
	uap.optname = optname;
	uap.optval = (uint64_t)optval;
	uap.optlen = optlen;

	net_errno = sys_setsockopt(td, &uap);

	return net_errno;
}

int sys_close(int fd) 
{
	struct sys_close_args {
		uint64_t fd;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

	auto sys_close = (int(*)(thread * td, sys_close_args * uap))sysents[SYS_close].sy_call;

	thread *td = curthread();
	td->td_retval[0] = 0;

	sys_close_args uap;
	uap.fd = fd;

	net_errno = sys_close(td, &uap);

	return net_errno;
}

int sys_netabort(int fd, int flags) 
{
	struct sys_netabort_args {
		uint64_t fd;
		uint64_t flags;
	};

	auto sv = (sysentvec*)resolve(addr_sysvec);
    sysent* sysents = sv->sv_table;

	auto sys_netabort = (int(*)(thread * td, sys_netabort_args * uap))sysents[SYS_abort2].sy_call;

	thread *td = curthread();
	td->td_retval[0] = 0;

	sys_netabort_args uap;
	uap.fd = fd;
	uap.flags = flags;

	net_errno = sys_netabort(td, &uap);

	return net_errno;
}

int Send(int Socket, const char* Data, int Length) {
	char* CurrentPosition = (char*)Data;

	int DataLeft = Length;
	int SentStatus = 0;
	while (DataLeft > 0) {
		int DataChunkSize = min(1024 * 2, DataLeft);
		SentStatus = sys_write(Socket, CurrentPosition, DataChunkSize);

		if (SentStatus == -1 ||  SentStatus == 0)
			break;

		DataLeft -= SentStatus;
		CurrentPosition += SentStatus;
	}

	if (SentStatus == -1)
		return 0;

	return 1;
}

int Receive(int Socket, char* Data, int Size) {
	char* CurrentPosition = (char*)Data;

	int DataLeft = Size;
	int ReceiveStatus = 0;

	while (DataLeft > 0) {
		int DataChunkSize = min(0x400, DataLeft);
		ReceiveStatus = sys_read(Socket, CurrentPosition, DataChunkSize);

		if (ReceiveStatus == -1 || ReceiveStatus == 0)
			break;

		CurrentPosition += ReceiveStatus;
		DataLeft -= ReceiveStatus;
	}

	if (ReceiveStatus == -1)
		return 0;

	return 1;
}