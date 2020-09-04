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

int(*sceKernelLoadStartModule)(const char *name, size_t argc, const void *argv, unsigned int flags, int pOpt, int pRes);
int(*sceSysUtilSendSystemNotificationWithText)(int messageType, const char* message);
int(*sceSysUtilSendNotificationRequest)(const char* message, int unk);

void resolveImports() {
    int library = 0;
    if (sys_dynlib_load_prx("libkernel.sprx", &library))
		if (sys_dynlib_load_prx("libkernel_web.sprx", &library))
			sys_dynlib_load_prx("libkernel_sys.sprx", &library);

	sys_dynlib_dlsym(library, "sceKernelLoadStartModule", &sceKernelLoadStartModule);

	int sysUtilHandle = sceKernelLoadStartModule("/system/common/lib/libSceSysUtil.sprx", 0, NULL, 0, 0, 0);
	sys_dynlib_dlsym(sysUtilHandle, "sceSysUtilSendSystemNotificationWithText", &sceSysUtilSendSystemNotificationWithText);
	sys_dynlib_dlsym(sysUtilHandle, "sceSysUtilSendNotificationRequest", &sceSysUtilSendNotificationRequest);
}