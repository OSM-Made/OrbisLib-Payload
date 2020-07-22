#include "../include/Lib.h"
#include "../include/net.h"
#include "../include/FreeBSD.h"

uint64_t getkernbase() {
	return __readmsr(0xC0000082) - __Xfast_syscall;
}

int DebugMode = 0;
uint32_t DebugIP = 0;
#define OSM
void printf(char* fmt, ...) {
	//if(DebugMode == 0)
	//	return;

	char buffer[0x400] = { 0 };

	va_list args;
	va_start(args, fmt);
    uint64_t kernbase = getkernbase();
    auto vsprintf = (int (*)(char* dst, const char* fmt, va_list ap))(kernbase + __vsprintf); 
	vsprintf(buffer, fmt, args);

	int sock = net_socket(AF_INET, SOCK_STREAM, 0);

	int optval = 1;
	net_setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

	struct sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	#ifdef OSM
	sockAddr.sin_port = 44582; //29975
	sockAddr.sin_addr.s_addr = IP(192, 168, 1, 166);
	#else
	sockAddr.sin_port = 29975; //
	sockAddr.sin_addr.s_addr = 0x7F8EC20A;
	#endif
	//sockAddr.sin_addr.s_addr = DebugIP;

	net_connect(sock, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr));
	net_send(sock, buffer, 0x400);
	net_close(sock);

	va_end(args);
}

char* strrchr(const char *cp, int ch)
{
    char *save;
    char c;

    for (save = (char *) 0; (c = *cp); cp++) {
	if (c == ch)
	    save = (char *) cp;
    }

    return save;
}

char* basename(const char *filename)
{
  char *p = strrchr(filename, '/');
  return p ? p + 1 : (char *)filename;
}