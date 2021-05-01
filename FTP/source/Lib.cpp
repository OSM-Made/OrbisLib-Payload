#include "../include/Lib.hpp"

//Kernel
unsigned int(*sceKernelSleep)(unsigned int seconds);
int(*sceKernelUsleep)(unsigned int microseconds);

//Mutex
int (*scePthreadMutexInit)(ScePthreadMutex *mutex, const ScePthreadMutexattr *attr, const char *name);
int (*scePthreadMutexDestroy)(ScePthreadMutex *mutex);
int (*scePthreadMutexLock)(ScePthreadMutex *mutex);
int (*scePthreadMutexTrylock)(ScePthreadMutex *mutex);
int (*scePthreadMutexTimedlock)(ScePthreadMutex *mutex, SceKernelUseconds usec);
int (*scePthreadMutexUnlock)(ScePthreadMutex *mutex);

//threads
int(*scePthreadCreate)(ScePthread *thread, const ScePthreadAttr *attr, void *(*entry) (void *), void *arg, const char *name);
void(*scePthreadExit)(void *value);
int(*scePthreadDetach)(ScePthread thread);
int(*scePthreadJoin)(ScePthread thread, void **value_ptr);

//Network
int (*sceNetSocket)(const char *, int, int, int);
int (*sceNetSocketClose)(int);
int (*sceNetConnect)(int, struct sockaddr *, int);
int (*sceNetSend)(int, const void *, size_t, int);
int (*sceNetBind)(int, struct sockaddr *, int);
int (*sceNetListen)(int, int);
int (*sceNetAccept)(int, struct sockaddr *, unsigned int *);
int (*sceNetRecv)(int, void *, size_t, int);
int (*sceNetSocketAbort)(int , int );

int (*sceNetGetsockname)(int, struct sockaddr *, unsigned int *);
int (*sceNetGetsockopt)(int s, int level, int optname, void * optval, socklen_t * optlen);
int (*sceNetSetsockopt)(int s, int level, int optname, const void *optval, socklen_t optlen);

const char (*sceNetInetNtop)(int af, const void *src, char *dst, int size);
int (*sceNetInetPton)(int af, const char *src, void *dst);

uint64_t (*sceNetHtonll)(uint64_t host64);
uint32_t (*sceNetHtonl)(uint32_t host32);
uint16_t (*sceNetHtons)(uint16_t host16);
uint64_t (*sceNetNtohll)(uint64_t net64);
uint32_t (*sceNetNtohl)(uint32_t net32);
uint16_t (*sceNetNtohs)(uint16_t net16);

int (*sceNetCtlInit)(void);
void (*sceNetCtlTerm)(void);
int (*sceNetCtlGetInfo)(int code, SceNetCtlInfo *info);

//sys
int(*sceKernelLoadStartModule)(const char *name, size_t argc, const void *argv, unsigned int flags, int pOpt, int pRes);
int(*sceKernelStopUnloadModule)(int handle, size_t args, const void *argp, uint32_t flags, int *pOpt, int *pRes);

//Notifications
int(*sceSysUtilSendSystemNotificationWithText)(int messageType, const char* message);
int(*sceSysUtilSendNotificationRequest)(const char* message, int unk);

int64_t (*sceKernelSendNotificationRequest)(int64_t unk1, char* Buffer, size_t size, int64_t unk2);

//libc
void *(*malloc)(size_t size);
void(*free)(void *ptr);
void *(*calloc)(size_t num, size_t size);
void *(*realloc)(void* ptr, size_t size);
void *(*memset)(void *destination, int value, size_t num);
void *(*memcpy)(void *destination, const void *source, size_t num);
int(*memcmp)(const void *s1, const void *s2, size_t n);
char *(*strcpy)(char *destination, const char *source);
char *(*strncpy)(char *destination, const char *source, size_t num);
char *(*strcat)(char *dest, const char *src);
char *(*strncat)(char *dest, const char *src, size_t n);
size_t(*strlen)(const char *s);
int(*strcmp)(const char *s1, const char *s2);
int(*strncmp)(const char *s1, const char *s2, size_t n);
int(*sprintf)(char *str, const char *format, ...);
int(*snprintf)(char *str, size_t size, const char *format, ...);
int(*vsprintf)(char * s, const char * format, va_list arg);
int(*sscanf)(const char *str, const char *format, ...);
char *(*strchr)(const char *s, int c);
char *(*strrchr)(const char *s, int c);
char *(*strstr)(char *str1, char *str2);
int(*atoi)(const char * str);
double(*atof)(const char * str);
double(*sin)(double x);
double(*cos)(double x);
double(*atan2)(double x, double y);
double(*sqrt)(double vec);
time_t (*time)(time_t *tloc);
struct tm *(*gmtime_s)(const time_t *timep, struct tm *result);
char *(*strdup)(const char *s);

int sys_dynlib_dlsym(int loadedModuleID, const char *name, void *destination) {
	return syscall(591, loadedModuleID, name, destination);
}

int sys_dynlib_load_prx(const char *name, int *idDestination) {
	return syscall(594, name, 0, idDestination, 0);
}

void LoadImports()
{
    //System Imports
    int Libkernel_library = 0;
    if (sys_dynlib_load_prx("libkernel.sprx", &Libkernel_library))
		if (sys_dynlib_load_prx("libkernel_web.sprx", &Libkernel_library))
			sys_dynlib_load_prx("libkernel_sys.sprx", &Libkernel_library);

	sys_dynlib_dlsym(Libkernel_library, "sceKernelSleep", &sceKernelSleep);
	sys_dynlib_dlsym(Libkernel_library, "sceKernelUsleep", &sceKernelUsleep);
    sys_dynlib_dlsym(Libkernel_library, "scePthreadCreate", &scePthreadCreate);
	sys_dynlib_dlsym(Libkernel_library, "scePthreadExit", &scePthreadExit);
	sys_dynlib_dlsym(Libkernel_library, "scePthreadDetach", &scePthreadDetach);
	sys_dynlib_dlsym(Libkernel_library, "scePthreadJoin", &scePthreadJoin);

	sys_dynlib_dlsym(Libkernel_library, "sceKernelLoadStartModule", &sceKernelLoadStartModule);
	sys_dynlib_dlsym(Libkernel_library, "sceKernelStopUnloadModule", &sceKernelStopUnloadModule);

	//Mutex
	sys_dynlib_dlsym(Libkernel_library, "scePthreadMutexInit", &scePthreadMutexInit);
	sys_dynlib_dlsym(Libkernel_library, "scePthreadMutexDestroy", &scePthreadMutexDestroy);
	sys_dynlib_dlsym(Libkernel_library, "scePthreadMutexLock", &scePthreadMutexLock);
	sys_dynlib_dlsym(Libkernel_library, "scePthreadMutexTrylock", &scePthreadMutexTrylock);
	sys_dynlib_dlsym(Libkernel_library, "scePthreadMutexTimedlock", &scePthreadMutexTimedlock);
	sys_dynlib_dlsym(Libkernel_library, "scePthreadMutexUnlock", &scePthreadMutexUnlock);
	sys_dynlib_dlsym(Libkernel_library, "sceKernelSendNotificationRequest", &sceKernelSendNotificationRequest);

    //Network Imports
    int Network_Library = 0;
	sys_dynlib_load_prx("libSceNet.sprx", &Network_Library);
	sys_dynlib_dlsym(Network_Library, "sceNetSocket", &sceNetSocket);
	sys_dynlib_dlsym(Network_Library, "sceNetSocketClose", &sceNetSocketClose);
	sys_dynlib_dlsym(Network_Library, "sceNetConnect", &sceNetConnect);
	sys_dynlib_dlsym(Network_Library, "sceNetSend", &sceNetSend);
    sys_dynlib_dlsym(Network_Library, "sceNetRecv", &sceNetRecv);
    sys_dynlib_dlsym(Network_Library, "sceNetGetsockopt", &sceNetGetsockopt);
	sys_dynlib_dlsym(Network_Library, "sceNetSetsockopt", &sceNetSetsockopt);
    sys_dynlib_dlsym(Network_Library, "sceNetListen", &sceNetListen);
    sys_dynlib_dlsym(Network_Library, "sceNetBind", &sceNetBind);
    sys_dynlib_dlsym(Network_Library, "sceNetAccept", &sceNetAccept);

	sys_dynlib_dlsym(Network_Library, "sceNetSocketAbort", &sceNetSocketAbort);
	sys_dynlib_dlsym(Network_Library, "sceNetGetsockname", &sceNetGetsockname);

	sys_dynlib_dlsym(Network_Library, "sceNetInetNtop", &sceNetInetNtop);
	sys_dynlib_dlsym(Network_Library, "sceNetInetPton", &sceNetInetPton);

	sys_dynlib_dlsym(Network_Library, "sceNetHtonll", &sceNetHtonll);
	sys_dynlib_dlsym(Network_Library, "sceNetHtonl", &sceNetHtonl);
	sys_dynlib_dlsym(Network_Library, "sceNetHtons", &sceNetHtons);
	sys_dynlib_dlsym(Network_Library, "sceNetNtohll", &sceNetNtohll);
	sys_dynlib_dlsym(Network_Library, "sceNetNtohl", &sceNetNtohl);
	sys_dynlib_dlsym(Network_Library, "sceNetNtohs", &sceNetNtohs);

	int Ctl_Library = 0;
	sys_dynlib_load_prx("libSceNetCtl.sprx", &Ctl_Library);
	sys_dynlib_dlsym(Ctl_Library, "sceNetCtlInit", &sceNetCtlInit);
	sys_dynlib_dlsym(Ctl_Library, "sceNetCtlTerm", &sceNetCtlTerm);
	sys_dynlib_dlsym(Ctl_Library, "sceNetCtlGetInfo", &sceNetCtlGetInfo);

    //Libc Imports
	int Libc_Library = 0;
	sys_dynlib_load_prx("libSceLibcInternal.sprx", &Libc_Library);
	sys_dynlib_dlsym(Libc_Library, "malloc", &malloc);
	sys_dynlib_dlsym(Libc_Library, "free", &free);
	sys_dynlib_dlsym(Libc_Library, "calloc", &calloc);
	sys_dynlib_dlsym(Libc_Library, "realloc", &realloc);
	sys_dynlib_dlsym(Libc_Library, "memset", &memset);
	sys_dynlib_dlsym(Libc_Library, "memcpy", &memcpy);
	sys_dynlib_dlsym(Libc_Library, "memcmp", &memcmp);
	sys_dynlib_dlsym(Libc_Library, "strcpy", &strcpy);
	sys_dynlib_dlsym(Libc_Library, "strncpy", &strncpy);
	sys_dynlib_dlsym(Libc_Library, "strcat", &strcat);
	sys_dynlib_dlsym(Libc_Library, "strncat", &strncat);
	sys_dynlib_dlsym(Libc_Library, "strlen", &strlen);
	sys_dynlib_dlsym(Libc_Library, "strcmp", &strcmp);
	sys_dynlib_dlsym(Libc_Library, "strncmp", &strncmp);
	sys_dynlib_dlsym(Libc_Library, "sprintf", &sprintf);
	sys_dynlib_dlsym(Libc_Library, "snprintf", &snprintf);
	sys_dynlib_dlsym(Libc_Library, "vsprintf", &vsprintf);
	sys_dynlib_dlsym(Libc_Library, "sscanf", &sscanf);
	sys_dynlib_dlsym(Libc_Library, "strchr", &strchr);
	sys_dynlib_dlsym(Libc_Library, "strrchr", &strrchr);
	sys_dynlib_dlsym(Libc_Library, "strstr", &strstr);
	sys_dynlib_dlsym(Libc_Library, "atoi", &atoi);
	sys_dynlib_dlsym(Libc_Library, "atof", &atof);
	sys_dynlib_dlsym(Libc_Library, "sin", &sin);
	sys_dynlib_dlsym(Libc_Library, "cos", &cos);
	sys_dynlib_dlsym(Libc_Library, "atan2", &atan2);
	sys_dynlib_dlsym(Libc_Library, "sqrt", &sqrt);
	sys_dynlib_dlsym(Libc_Library, "time", &time);
	sys_dynlib_dlsym(Libc_Library, "gmtime_s", &gmtime_s);
	sys_dynlib_dlsym(Libc_Library, "strdup", &strdup);
}

void Sleep(unsigned int milliseconds) {
	sceKernelUsleep(milliseconds * 1000);
}
enum NotifyType
{
	NotificationRequest = 0,
	SystemNotification = 1,
	SystemNotificationWithUserId = 2,
	SystemNotificationWithDeviceId = 3,
	SystemNotificationWithDeviceIdRelatedToUser = 4,
	SystemNotificationWithText = 5,
	SystemNotificationWithTextRelatedToUser = 6,
	SystemNotificationWithErrorCode = 7,
	SystemNotificationWithAppId = 8,
	SystemNotificationWithAppName = 9,
	SystemNotificationWithAppInfo = 9,
	SystemNotificationWithAppNameRelatedToUser = 10,
	SystemNotificationWithParams = 11,
	SendSystemNotificationWithUserName = 12,
	SystemNotificationWithUserNameInfo = 13,
	SendAddressingSystemNotification = 14,
	AddressingSystemNotificationWithDeviceId = 15,
	AddressingSystemNotificationWithUserName = 16,
	AddressingSystemNotificationWithUserId = 17,

	UNK_1 = 100,
	TrcCheckNotificationRequest = 101,
	NpDebugNotificationRequest = 102,
	UNK_2 = 102,
};

struct NotifyBuffer
{ //Naming may be incorrect.
	NotifyType Type;		//0x00 
	int ReqId;				//0x04
	int Priority;			//0x08
	int MsgId;				//0x0C
	int TargetId;			//0x10
	int UserId;				//0x14
	int unk1;				//0x18
	int unk2;				//0x1C
	int AppId;				//0x20
	int ErrorNum;			//0x24
	int unk3;				//0x28
	char UseIconImageUri; 	//0x2C
	char Message[1024]; 	//0x2D
	char Uri[1024]; 		//0x42D
	char unkstr[1024];		//0x82D
}; //Size = 0xC30

void Notify(const char* MessageFMT, ...)
	{
		NotifyBuffer Buffer;

		//Create full string from va list.
		va_list args;
		va_start(args, MessageFMT);
		vsprintf(Buffer.Message, MessageFMT, args);
		va_end(args);

		//Populate the notify buffer.
		Buffer.Type = NotifyType::NotificationRequest; //this one is just a standard one and will print what ever is stored at the buffer.Message.
		Buffer.unk3 = 0;
		Buffer.UseIconImageUri = 1; //Bool to use a custom uri.
		Buffer.TargetId = -1; //Not sure if name is correct but is always set to -1.
		strcpy(Buffer.Uri, "https://i.imgur.com/SJPIBGG.png"); //Copy the uri to the buffer.

	  //From user land we can call int64_t sceKernelSendNotificationRequest(int64_t unk1, char* Buffer, size_t size, int64_t unk2) which is a libkernel import.
		sceKernelSendNotificationRequest(0, (char*)&Buffer, 3120, 0);

		//What sceKernelSendNotificationRequest is doing is opening the device "/dev/notification0" or "/dev/notification1"
		// and writing the NotifyBuffer we created to it. Somewhere in ShellUI it is read and parsed into a json which is where
		// I found some clues on how to build the buffer.
	}

#define OSM
void printf(const char *fmt, ...) {
	/*char buffer[0x400] = { 0 };
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	int sock = sceNetSocket("PS4 Output", AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int optval = 1;
	sceNetSetsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

	struct sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	#ifdef OSM
	sockAddr.sin_port = 44582; //29975
	sockAddr.sin_addr.s_addr = IP(192, 168, 1, 64); //0x7F8EC20A
	#else
	sockAddr.sin_port = 29975; //
	sockAddr.sin_addr.s_addr = 0x7F8EC20A;
	#endif

	sceNetConnect(sock, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr_in));
	sceNetSend(sock, buffer, 0x400, 0);
	sceNetSocketClose(sock);

	va_end(args);*/
}

void printf2(const char *fmt, ...) {
	/*char buffer[0x400] = { 0 };
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	int sock = sceNetSocket("PS4 Output", AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int optval = 1;
	sceNetSetsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

	struct sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = 44582; //29975
	sockAddr.sin_addr.s_addr = IP(192, 168, 1, 64); //0x7F8EC20A

	sceNetConnect(sock, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr_in));
	sceNetSend(sock, buffer, 0x400, 0);
	sceNetSocketClose(sock);

	va_end(args);*/
}