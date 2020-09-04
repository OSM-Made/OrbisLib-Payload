#pragma once

#include <stdint.h>
#include <stdarg.h>

#include "types.hpp"
#include "syscall.hpp"

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

struct moduleInfo {
    size_t size; // 0x0
    char name[32]; // 0x8
    char padding1[0xe0]; // 0x28
    void *codeBase; // 0x108
    unsigned int codeSize; // 0x110
    void *dataBase; // 0x118
    unsigned int dataSize; // 0x120
    char padding2[0x3c]; // 0x124
};

/* info code */
#define SCE_NET_CTL_INFO_DEVICE			1
#define SCE_NET_CTL_INFO_ETHER_ADDR		2
#define SCE_NET_CTL_INFO_MTU			3
#define SCE_NET_CTL_INFO_LINK			4
#define SCE_NET_CTL_INFO_BSSID			5
#define SCE_NET_CTL_INFO_SSID			6
#define SCE_NET_CTL_INFO_WIFI_SECURITY		7
#define SCE_NET_CTL_INFO_RSSI_DBM		8
#define SCE_NET_CTL_INFO_RSSI_PERCENTAGE	9
#define SCE_NET_CTL_INFO_CHANNEL		10
#define SCE_NET_CTL_INFO_IP_CONFIG		11
#define SCE_NET_CTL_INFO_DHCP_HOSTNAME		12
#define SCE_NET_CTL_INFO_PPPOE_AUTH_NAME	13
#define SCE_NET_CTL_INFO_IP_ADDRESS		14
#define SCE_NET_CTL_INFO_NETMASK		15
#define SCE_NET_CTL_INFO_DEFAULT_ROUTE		16
#define SCE_NET_CTL_INFO_PRIMARY_DNS		17
#define SCE_NET_CTL_INFO_SECONDARY_DNS		18
#define SCE_NET_CTL_INFO_HTTP_PROXY_CONFIG	19
#define SCE_NET_CTL_INFO_HTTP_PROXY_SERVER	20
#define SCE_NET_CTL_INFO_HTTP_PROXY_PORT	21
#define SCE_NET_CTL_INFO_RESERVED1		22
#define SCE_NET_CTL_INFO_RESERVED2		23

#define SCE_NET_ETHER_ADDR_LEN 6

typedef struct SceNetEtherAddr {
	uint8_t data[SCE_NET_ETHER_ADDR_LEN];
} SceNetEtherAddr;

#define SCE_NET_CTL_SSID_LEN		(32 + 1)
#define SCE_NET_CTL_HOSTNAME_LEN	(255 + 1)
#define SCE_NET_CTL_AUTH_NAME_LEN	(127 + 1)
#define SCE_NET_CTL_IPV4_ADDR_STR_LEN	(16)

typedef union SceNetCtlInfo {
	uint32_t device;
	SceNetEtherAddr ether_addr;
	uint32_t mtu;
	uint32_t link;
	SceNetEtherAddr bssid;
	char ssid[SCE_NET_CTL_SSID_LEN];
	uint32_t wifi_security;
	uint8_t rssi_dbm;
	uint8_t rssi_percentage;
	uint8_t channel;
	uint32_t ip_config;
	char dhcp_hostname[SCE_NET_CTL_HOSTNAME_LEN];
	char pppoe_auth_name[SCE_NET_CTL_AUTH_NAME_LEN];
	char ip_address[SCE_NET_CTL_IPV4_ADDR_STR_LEN];
	char netmask[SCE_NET_CTL_IPV4_ADDR_STR_LEN];
	char default_route[SCE_NET_CTL_IPV4_ADDR_STR_LEN];
	char primary_dns[SCE_NET_CTL_IPV4_ADDR_STR_LEN];
	char secondary_dns[SCE_NET_CTL_IPV4_ADDR_STR_LEN];
	uint32_t http_proxy_config;
	char http_proxy_server[SCE_NET_CTL_HOSTNAME_LEN];
	uint16_t http_proxy_port;
} SceNetCtlInfo;

typedef unsigned int socklen_t;
typedef void *ScePthread;
typedef void *ScePthreadAttr;
typedef void *ScePthreadMutex;
typedef void *ScePthreadMutexattr;

//kernel
extern unsigned int (*sceKernelSleep)(unsigned int seconds);
extern int (*sceKernelUsleep)(unsigned int microseconds);

//Mutex
extern int (*scePthreadMutexInit)(ScePthreadMutex *mutex, const ScePthreadMutexattr *attr, const char *name);
extern int (*scePthreadMutexDestroy)(ScePthreadMutex *mutex);
extern int (*scePthreadMutexLock)(ScePthreadMutex *mutex);
extern int (*scePthreadMutexTrylock)(ScePthreadMutex *mutex);
extern int (*scePthreadMutexTimedlock)(ScePthreadMutex *mutex, SceKernelUseconds usec);
extern int (*scePthreadMutexUnlock)(ScePthreadMutex *mutex);

//threads
extern int (*scePthreadCreate)(ScePthread *thread, const ScePthreadAttr *attr, void *(*entry) (void *), void *arg, const char *name);
extern void (*scePthreadExit)(void *value);
extern int (*scePthreadDetach)(ScePthread thread);
extern int (*scePthreadJoin)(ScePthread thread, void **value_ptr);

//network
extern int (*sceNetSocket)(const char *, int, int, int);
extern int (*sceNetSocketClose)(int);
extern int (*sceNetConnect)(int, struct sockaddr *, int);
extern int (*sceNetSend)(int, const void *, size_t, int);
extern int (*sceNetBind)(int, struct sockaddr *, int);
extern int (*sceNetListen)(int, int);
extern int (*sceNetAccept)(int, struct sockaddr *, unsigned int *);
extern int (*sceNetRecv)(int, void *, size_t, int);
extern int (*sceNetSocketAbort)(int , int );

extern int (*sceNetGetsockname)(int, struct sockaddr *, unsigned int *);
extern int (*sceNetGetsockopt)(int s, int level, int optname, void * optval, socklen_t * optlen);
extern int (*sceNetSetsockopt)(int s, int level, int optname, const void *optval, socklen_t optlen);

extern const char (*sceNetInetNtop)(int af, const void *src, char *dst, int size);
extern int (*sceNetInetPton)(int af, const char *src, void *dst);

extern uint64_t (*sceNetHtonll)(uint64_t host64);
extern uint32_t (*sceNetHtonl)(uint32_t host32);
extern uint16_t (*sceNetHtons)(uint16_t host16);
extern uint64_t (*sceNetNtohll)(uint64_t net64);
extern uint32_t (*sceNetNtohl)(uint32_t net32);
extern uint16_t (*sceNetNtohs)(uint16_t net16);

extern int (*sceNetCtlInit)(void);
extern void (*sceNetCtlTerm)(void);
extern int (*sceNetCtlGetInfo)(int code, SceNetCtlInfo *info);

//sys
extern int(*sceKernelLoadStartModule)(const char *name, size_t argc, const void *argv, unsigned int flags, int, int);
extern int(*sceKernelStopUnloadModule)(int handle, size_t args, const void *argp, uint32_t flags, int *pOpt, int *pRes);

//notifications
extern int(*sceSysUtilSendSystemNotificationWithText)(int messageType, const char* message);
extern int(*sceSysUtilSendNotificationRequest)(const char* message, int unk);

//libc
extern void *(*malloc)(size_t size);
extern void (*free)(void *ptr);
extern void *(*calloc)(size_t num, size_t size);
extern void *(*realloc)(void* ptr, size_t size);
extern void *(*memset)(void *destination, int value, size_t num);
extern void *(*memcpy)(void *destination, const void *source, size_t num);
extern int (*memcmp)(const void *s1, const void *s2, size_t n);
extern char *(*strcpy)(char *destination, const char *source);
extern char *(*strncpy)(char *destination, const char *source, size_t num);
extern char *(*strcat)(char *dest, const char *src);
extern char *(*strncat)(char *dest, const char *src, size_t n);
extern size_t (*strlen)(const char *s);
extern int (*strcmp)(const char *s1, const char *s2);
extern int (*strncmp)(const char *s1, const char *s2, size_t n);
extern int (*sprintf)(char *str, const char *format, ...);
extern int (*snprintf)(char *str, size_t size, const char *format, ...);
extern int (*vsprintf)(char * s, const char * format, va_list arg);
extern int (*sscanf)(const char *str, const char *format, ...);
extern char *(*strchr)(const char *s, int c);
extern char *(*strrchr)(const char *s, int c);
extern char *(*strstr)(char *str1, char *str2);
extern int (*atoi)(const char * str);
extern double (*atof)(const char * str);
extern double(*sin)(double x);
extern double(*cos)(double x);
extern double(*atan2)(double x, double y);
extern double(*sqrt)(double vec);
extern time_t (*time)(time_t *tloc);
extern struct tm *(*gmtime_s)(const time_t *timep, struct tm *result);
extern char *(*strdup)(const char *s);

extern int (*sysctlbyname)(char *name, char *oldval, size_t *oldlen, char *newval, size_t newlen);

int sys_dynlib_dlsym(int loadedModuleID, const char *name, void *destination);
int sys_dynlib_load_prx(const char *name, int *idDestination);

void LoadImports();
void Sleep(unsigned int milliseconds);
void printf(const char *fmt, ...);

inline void* operator new(long unsigned int size) { return malloc(size); }
inline void* operator new[](long unsigned int size) { return malloc(size); }
inline void operator delete(void* ptr) { free(ptr); }
inline void operator delete[](void* ptr) { free(ptr); }