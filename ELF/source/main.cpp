#include "Main.hpp"
#include "Util/Detour.hpp"
#include "Util/Kernel.hpp"
#include "Util/FileIO.hpp"

extern "C"
{
	#include "sys/mount.h"
}

OrbisLib* orbisLib = NULL;
HelperManager* pHelperManager = NULL;

extern "C" void _main(uint64_t* p)
{
	Install_Patches();
	ResolveFunctions();

	//Start up the main orbis lib API.
	orbisLib = new OrbisLib();

	//Start up the Helper Manager
	pHelperManager = new HelperManager();

	//Start up the DebugLogger
    //StartDebugLogger();

	DebugLog(LOGTYPE_INFO, "API Loaded!!!\nProcess Functions: %d\nDebugging Functions %d\n Kernel Functions: %d\nTarget Functions: %d\nTotal API Calls: %d", (PROC_END - PROC_START) - 2, (DBG_END - DBG_START) - 2, (KERN_END - KERN_START) - 2, (TARGET_END - TARGET_START) - 2, TARGET_END - 8);

	pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest("OrbisLib v2.0 Loaded!\nMade by OSM");

}
