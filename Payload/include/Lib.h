#pragma once

#include "types.h"

#define AF_INET 0x0002

#define IN_ADDR_ANY 0

#define SOCK_STREAM 1
#define SOCK_DGRAM 2

#define SOL_SOCKET 0xffff
#define SO_NBIO 0x1200

#define MSG_DONTWAIT 0x80
#define MSG_WAITALL 0x40

#define IPPROTO_TCP 6
#define TCP_NODELAY 1

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

typedef unsigned int socklen_t;
typedef void *ScePthread;
typedef void *ScePthreadAttr;

#define IP(a, b, c, d) (((a) << 0) + ((b) << 8) + ((c) << 16) + ((d) << 24))

uint64_t getkernbase();
void *alloc(uint32_t size);
void dealloc(void *addr);
int sys_dynlib_dlsym(int loadedModuleID, const char *name, void *destination);
int sys_dynlib_load_prx(const char *name, int *idDestination);
void resolveImports();

extern int(*sceNetSocket)(const char *, int, int, int);
extern int(*sceNetSocketClose)(int);
extern int(*sceNetConnect)(int, struct sockaddr *, int);
extern int(*sceNetSend)(int, const void *, size_t, int);

extern int(*sceKernelLoadStartModule)(const char *name, size_t argc, const void *argv, unsigned int flags, int pOpt, int pRes);

extern int(*sceSysUtilSendSystemNotificationWithText)(int messageType, const char* message);

void printf(const char *fmt, ...);