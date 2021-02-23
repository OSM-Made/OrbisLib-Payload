#include "Main.hpp"
#include "Util/Detour.hpp"
#include "Util/Kernel.hpp"
#include "Util/FileIO.hpp"

OrbisLib* orbisLib = NULL;
HelperManager* pHelperManager = NULL;
TargetComms* pTargetComms = NULL;

struct NotifyBuffer
{
	int unk1[11];
	char unk2;
	char MessageBuffer[3075];
	//uint64_t unk3; //Callback??
};

char byte[sizeof(NotifyBuffer)];

void TestCallback()
{
	
}

void TestNotify(const char* Msg)
{
	NotifyBuffer Buffer;

	//Buffer.unk3 = (uint64_t)&TestCallback;
	Buffer.unk1[0] = 0;
	Buffer.unk1[10] = 0;
	Buffer.unk2 = 0;
	Buffer.unk1[4] = -1;
	strcpy(Buffer.MessageBuffer, (char*)Msg);

	unsigned int device = sys_fopen("/dev/notification0", 5, 0);

	if(device)
	{
		//sys_fwrite(device, &Buffer, 3120);

		sys_fclose(device);
	}
}

extern "C" void _main(uint64_t* p)
{
	Install_Patches();
	ResolveFunctions();

	//Target Comms used for sending info to the host.
    pTargetComms = new TargetComms();

	//Start up the main orbis lib API.
	orbisLib = new OrbisLib();

	//Start up the thread once the kproc has started.
	pTargetComms->StartUpThread();

	//Start up the Helper Manager
	pHelperManager = new HelperManager();

	DebugLog(LOGTYPE_INFO, "API Loaded!!!\nProcess Functions: %d\nDebugging Functions %d\nKernel Functions: %d\nTarget Functions: %d\nTotal API Calls: %d", (PROC_END - PROC_START) - 2, (DBG_END - DBG_START) - 2, (KERN_END - KERN_START) - 2, (TARGET_END - TARGET_START) - 2, TARGET_END - 8);

	//pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest("OrbisLib v%i.%i Loaded!\nMade by OSM", ORBISLIB_MAJOR_VERSION, ORBISLIB_MINOR_VERSION);

	TestNotify("Test");

	kprintf("Hello world from OrbisLib!!!\n");

	//sceKernelIccIndicatorStandbyBoot();
}
