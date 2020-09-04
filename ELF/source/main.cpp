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

	proc* proc = proc_find_by_name("SceShellCore");
	if(proc)
	{
		Log("Text:\nBase %llX Size %llX", proc->p_vmspace->vm_taddr, proc->p_vmspace->vm_tsize * PAGE_SIZE);
		Log("Data:\nBase %llX Size %llX", proc->p_vmspace->vm_daddr, proc->p_vmspace->vm_dsize * PAGE_SIZE);
	}

	//Log("Hello from Kernel Land!!!");
}
