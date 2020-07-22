#include "../include/net.h"
#include "../include/Resolve.h"
#include "../include/Lib.h"
#include "../include/FreeBSD.h"

int net_errno;

void net_enable_copy_checks() {
 	uint64_t kernbase = getkernbase();
 	uint64_t CR0 = __readcr0();

 	uint16_t* copyin1 = (uint16_t*)(kernbase + 0x1EA758); //0x14A8D8
 	uint16_t* copyin2 = (uint16_t*)(kernbase + 0x1EA767); //0x14A8E7
 	uint16_t* copyout1 = (uint16_t*)(kernbase + 0x1EA66B); //0x14A7EB
 	uint16_t* copyout2 = (uint16_t*)(kernbase + 0x1EA673); //0x14A7F3
 	uint16_t* copyout3 = (uint16_t*)(kernbase + 0x1EA682); //0x14A802

 	__writecr0(CR0 & ~CR0_WP);
 	*copyin1 = 0x4672;
 	*copyin2 = 0x3777;
 	*copyout1 = 0x6375;
 	*copyout2 = 0x5B72;
 	*copyout3 = 0x4C77;
 	__writecr0(CR0);
}

void net_disable_copy_checks() {
 	uint64_t kernbase = getkernbase();
 	uint64_t CR0 = __readcr0();

 	uint16_t* copyin1 = (uint16_t*)(kernbase + 0x1EA758); //0x14A8D8
 	uint16_t* copyin2 = (uint16_t*)(kernbase + 0x1EA767); //0x14A8E7
 	uint16_t* copyout1 = (uint16_t*)(kernbase + 0x1EA66B); //0x14A7EB
 	uint16_t* copyout2 = (uint16_t*)(kernbase + 0x1EA673); //0x14A7F3
 	uint16_t* copyout3 = (uint16_t*)(kernbase + 0x1EA682); //0x14A802

 	__writecr0(CR0 & ~CR0_WP);
 	*copyin1 = 0x9090;
 	*copyin2 = 0x9090;
 	*copyout1 = 0x9090;
 	*copyout2 = 0x9090;
 	*copyout3 = 0x9090;
 	__writecr0(CR0);
}

int net_socket(int domain, int type, int protocol) 
{
	struct sys_socket_args {
		uint64_t domain;
		uint64_t type;
		uint64_t protocol;
	};

	uint64_t kernbase = getkernbase();

	int (*sys_socket)(thread * td, sys_socket_args * uap) = (int(*)(thread * td, sys_socket_args * uap))(kernbase + __sys_socket);

	thread *td = curthread();

	sys_socket_args uap;
	uap.domain = domain;
	uap.type = type;
	uap.protocol = protocol;

	net_errno = sys_socket(td, &uap);

	return td->td_retval[0];
}

int net_connect(int sockfd, sockaddr *addr, int addrlen) 
{
	struct sys_connect_args {
		uint64_t sockfd;
		uint64_t name;
		uint64_t namelen;
	};

	uint64_t kernbase = getkernbase();

	int (*sys_connect)(thread * td, sys_connect_args * uap) = (int (*)(thread * td, sys_connect_args * uap))(kernbase + __sys_connect);

	thread *td = curthread();

	sys_connect_args uap;
	uap.sockfd = sockfd;
	uap.name = (uint64_t)addr;
	uap.namelen = addrlen;

	net_errno = sys_connect(td, &uap);

	return net_errno;
}

int net_bind(int sockfd, sockaddr *addr, int addrlen) 
{
	struct sys_bind_args {
		uint64_t sockfd;
		uint64_t name;
		uint64_t namelen;
	};

	uint64_t kernbase = getkernbase();

	int (*sys_bind)(thread * td, sys_bind_args * uap) = (int (*)(thread * td, sys_bind_args * uap))(kernbase + __sys_bind);

	thread *td = curthread();

	sys_bind_args uap;
	uap.sockfd = sockfd;
	uap.name = (uint64_t)addr;
	uap.namelen = addrlen;

	net_errno = sys_bind(td, &uap);

	return net_errno;
}

int net_listen(int sockfd, int backlog) 
{
	struct sys_listen_args {
		uint64_t sockfd;
		uint64_t backlog;
	};

	uint64_t kernbase = getkernbase();

	int (*sys_listen)(thread * td, sys_listen_args * uap) = (int (*)(thread * td, sys_listen_args * uap))(kernbase + __sys_listen);

	thread *td = curthread();

	sys_listen_args uap;
	uap.sockfd = sockfd;
	uap.backlog = backlog;

	net_errno = sys_listen(td, &uap);

	return net_errno;
}

int net_accept(int sockfd, sockaddr *addr, int *addrlen) 
{
	struct sys_accept_args {
		uint64_t sockfd;
		uint64_t name;
		uint64_t namelen;
	};

	uint64_t kernbase = getkernbase();

	int (*sys_accept)(thread * td, sys_accept_args * uap) = (int (*)(thread * td, sys_accept_args * uap))(kernbase + __sys_accept);

	thread *td = curthread();

	sys_accept_args uap;
	uap.sockfd = sockfd;
	uap.name = (uint64_t)addr;
	uap.namelen = (uint64_t)addrlen;

	net_errno = sys_accept(td, &uap);

	return td->td_retval[0];
}

int net_recv(int fd, void *buf, uint64_t len) 
{
	struct sys_read_args {
		uint64_t fd;
		uint64_t buf;
		uint64_t nbyte;
	};

	uint64_t kernbase = getkernbase();

	int (*sys_read)(thread * td, sys_read_args * uap) = (int (*)(thread * td, sys_read_args * uap))(kernbase + __sys_read);

	thread *td = curthread();

	sys_read_args uap;
	uap.fd = fd;
	uap.buf = (uint64_t)buf;
	uap.nbyte = len;

	net_errno = sys_read(td, &uap);

	return td->td_retval[0];
}

int net_send(int fd, const void *buf, uint64_t len) 
{
	struct sys_write_args {
		uint64_t fd;
		uint64_t buf;
		uint64_t nbyte;
	};

	uint64_t kernbase = getkernbase();

	int (*sys_write)(thread * td, sys_write_args * uap) = (int (*)(thread * td, sys_write_args * uap))(kernbase + __sys_send);

	thread *td = curthread();

	sys_write_args uap;
	uap.fd = fd;
	uap.buf = (uint64_t)buf;
	uap.nbyte = len;

	net_errno = sys_write(td, &uap);

	return td->td_retval[0];
}

int net_setsockopt(int s, int level, int optname, const void *optval, uint32_t optlen) 
{
	struct sys_setsockopt_args {
		uint64_t fd;
		uint64_t level;
		uint64_t optname;
		uint64_t optval;
		uint64_t optlen;
	};

	uint64_t kernbase = getkernbase();

	int (*sys_setsockopt)(thread * td, sys_setsockopt_args * uap) = (int (*)(thread * td, sys_setsockopt_args * uap))(kernbase + __sys_setsockopt);

	thread *td = curthread();

	sys_setsockopt_args uap;
	uap.fd = s;
	uap.level = level;
	uap.optname = optname;
	uap.optval = (uint64_t)optval;
	uap.optlen = optlen;

	net_errno = sys_setsockopt(td, &uap);

	return net_errno;
}

int net_close(int fd) 
{
	struct sys_close_args {
		uint64_t fd;
	};

	uint64_t kernbase = getkernbase();

	int (*sys_close)(thread * td, sys_close_args * uap) = (int (*)(thread * td, sys_close_args * uap))(kernbase + __sys_close);

	thread *td = curthread();

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

	uint64_t kernbase = getkernbase();

	int (*sys_netabort)(thread * td, sys_netabort_args * uap) = (int (*)(thread * td, sys_netabort_args * uap))(kernbase + __sys_netabort);

	thread *td = curthread();

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
		SentStatus = net_send(Socket, CurrentPosition, DataChunkSize);

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
		ReceiveStatus = net_recv(Socket, CurrentPosition, DataChunkSize);

		if (ReceiveStatus == -1 || ReceiveStatus == 0)
			break;

		CurrentPosition += ReceiveStatus;
		DataLeft -= ReceiveStatus;
	}

	if (ReceiveStatus == -1)
		return 0;

	return 1;
}
