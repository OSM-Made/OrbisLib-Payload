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

	//Getting Console info
	struct nvs_console_info {
		char moboserial[14];
		char _fill1[0x22];
		char serial[10];
		char _fill2[7];
		char model[13];
	} __attribute__((packed));

	nvs_console_info info;
	icc_nvs_read(2, 0, sizeof(nvs_console_info), (unsigned char*)&info);

	//pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest("Motherboard Serial: %.*s", info.moboserial);
	//pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest("Serial: %.*s", info.serial);
	pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest(info.model);

	//Getting Mac Address of lan
	unsigned char MACAdress[6];
	icc_nvs_read(0, 0x21, sizeof(MACAdress), (unsigned char*)&MACAdress);
	pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest("%02X:%02X:%02X:%02X:%02X:%02X", MACAdress[0], MACAdress[1], MACAdress[2], MACAdress[3], MACAdress[4], MACAdress[5]);
	//DebugLog(LOGTYPE_INFO, "%02X:%02X:%02X:%02X:%02X:%02X", MACAdress[0], MACAdress[1], MACAdress[2], MACAdress[3], MACAdress[4], MACAdress[5]);

	pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest("OrbisLib v%i.%i Loaded!\nMade by OSM", ORBISLIB_MAJOR_VERSION, ORBISLIB_MINOR_VERSION);
}
