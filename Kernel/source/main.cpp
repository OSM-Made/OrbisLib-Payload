#include "Main.hpp"
#include "Util/Detour.hpp"
#include "Util/Kernel.hpp"
#include "Util/FileIO.hpp"

OrbisLib* orbisLib = NULL;
HelperManager* pHelperManager = NULL;
TargetComms* pTargetComms = NULL;

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

	//Enable UART
	//uint8_t Uart = 1;
	//icc_nvs_write(4, 0x31F, 0x1, &Uart);

	uint8_t Uart = 0;
	icc_nvs_write(4, 0x1600, 0x1, &Uart);

	//for(int i = 0; i < 6; i++)
	//	Log("Value = %d%%\n", *(int32_t*)(resolve(0x2381C48) + (i * 0x4)));

	pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest("OrbisLib v%i.%i Loaded!\nMade by OSM", ORBISLIB_MAJOR_VERSION, ORBISLIB_MINOR_VERSION);

	kprintf("Hello world from OrbisLib!!!\n");
}
