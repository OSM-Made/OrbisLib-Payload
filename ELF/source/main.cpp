#include "Main.hpp"

OrbisLib* orbisLib = NULL;
HelperManager* pHelperManager = NULL;

extern "C" void _main(uint64_t* p)
{
	Install_Patches();
	ResolveFunctions();

	Log("Hello from Kernel Land!!!");

	orbisLib = new OrbisLib();

	//Start up the DebugLogger
    StartDebugLogger();

	//Start up the Helper Manager
	pHelperManager = new HelperManager();
}
