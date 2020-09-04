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
    StartDebugLogger();

	pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest("OrbisLib v2.0 Loaded!\nMade by OSM");
	
}
