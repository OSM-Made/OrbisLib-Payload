#include "main.h"

uint64_t getkernbase() {
	return __readmsr(0xC0000082) - __Xfast_syscall;
}

void *alloc(uint32_t size) {
	return malloc(size, M_TEMP, 2);
}

void dealloc(void *addr) {
	free(addr, M_TEMP);
}

int sys_dynlib_dlsym(int loadedModuleID, const char *name, void *destination) {
	return syscall(591, loadedModuleID, name, destination);
}

int sys_dynlib_load_prx(const char *name, int *idDestination) {
	return syscall(594, name, 0, idDestination, 0);
}

int(*sceNetSocket)(const char *, int, int, int);
int(*sceNetSocketClose)(int);
int(*sceNetConnect)(int, struct sockaddr *, int);
int(*sceNetSend)(int, const void *, size_t, int);

int(*sceKernelLoadStartModule)(const char *name, size_t argc, const void *argv, unsigned int flags, int pOpt, int pRes);

int(*sceSysUtilSendSystemNotificationWithText)(int messageType, const char* message);

void resolveImports() {
    int library = 0;
    if (sys_dynlib_load_prx("libkernel.sprx", &library))
		if (sys_dynlib_load_prx("libkernel_web.sprx", &library))
			sys_dynlib_load_prx("libkernel_sys.sprx", &library);

	sys_dynlib_dlsym(library, "sceKernelLoadStartModule", &sceKernelLoadStartModule);

	int Network_Library = 0;
	sys_dynlib_load_prx("libSceNet.sprx", &Network_Library);
	sys_dynlib_dlsym(Network_Library, "sceNetSocket", &sceNetSocket);
	sys_dynlib_dlsym(Network_Library, "sceNetSocketClose", &sceNetSocketClose);
	sys_dynlib_dlsym(Network_Library, "sceNetConnect", &sceNetConnect);
	sys_dynlib_dlsym(Network_Library, "sceNetSend", &sceNetSend);

	int sysUtilHandle = sceKernelLoadStartModule("/system/common/lib/libSceSysUtil.sprx", 0, NULL, 0, 0, 0);
	sys_dynlib_dlsym(sysUtilHandle, "sceSysUtilSendSystemNotificationWithText", &sceSysUtilSendSystemNotificationWithText);


}

#define OSM
void printf(const char *fmt, ...) {
	char buffer[0x400] = { 0 };
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	int sock = sceNetSocket("PS4 Output", AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //int optval = 1;
	//sceNetSetsockopt(sock, SOL_SOCKET, 0x0800, (void *)&optval, sizeof(int));

	struct sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	#ifdef OSM
	sockAddr.sin_port = 44582; //29975
	sockAddr.sin_addr.s_addr = IP(192, 168, 1, 166); //0x7F8EC20A
	#else
	sockAddr.sin_port = 29975; //
	sockAddr.sin_addr.s_addr = 0x7F8EC20A;
	#endif

	sceNetConnect(sock, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr_in));
	sceNetSend(sock, buffer, 0x400, 0);
	sceNetSocketClose(sock);

	va_end(args);
}