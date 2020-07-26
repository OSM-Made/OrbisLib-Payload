#include "Main.hpp"

OrbisLib* kproc = NULL;

extern "C" void _main(uint64_t* p)
{
	Install_Patches();
	ResolveFunctions();

	Log("Hello from Kernel Land!!!");

	kproc = new OrbisLib();
}
