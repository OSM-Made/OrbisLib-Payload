#include "../include/main.hpp"
#include "../include/FTP.hpp"
#include "../include/memory.hpp"

extern "C" void _main(void *arg) {
	LoadImports();

	//sceSystemStateMgrEnterStandby(1);

	/*uint64_t (*sceShellCoreUtilRequestShutdown)(int unk);
	int libSceSystemService = sceKernelLoadStartModule("libSceSystemService.sprx", 0, NULL, 0, 0, 0);
	sys_dynlib_dlsym(libSceSystemService, "sceShellCoreUtilRequestShutdown", &sceShellCoreUtilRequestShutdown);

	if(sceShellCoreUtilRequestShutdown == 0)
		sceSysUtilSendNotificationRequest("Suck my left nut sack faggot", 0);
	else
	{
		sceSysUtilSendNotificationRequest("We can call that ass hole", 0);

		int res = sceShellCoreUtilRequestShutdown(0x18);
		if(res < 0)
			sceSysUtilSendNotificationRequest("We failed that hoe", 0);
	}*/

	InitFTP();
}