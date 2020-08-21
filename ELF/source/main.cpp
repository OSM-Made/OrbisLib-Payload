#include "Main.hpp"
#include "Util/Detour.hpp"
#include "Util/Kernel.hpp"

extern "C"
{
	#include <sys/reboot.h>
}

OrbisLib* orbisLib = NULL;
HelperManager* pHelperManager = NULL;
OrbisShellCode* FTPShellCode = NULL;
int FTPSprxHandle;

extern uint8_t OrbisFTP[];
extern int32_t OrbisFTPSize;

extern "C" void _main(uint64_t* p)
{
	Install_Patches();
	ResolveFunctions();

	Log("Hello from Kernel Land!!!");

	//Start up the main orbis lib API.
	orbisLib = new OrbisLib();

	//Start up the Helper Manager
	pHelperManager = new HelperManager();

	//Start up the DebugLogger
    StartDebugLogger();

	//Start up the FTP Server.
	proc* proc = proc_find_by_name("SceRemotePlay");
	if(proc) 
	{
        //Give Root FS Perms
		filedesc* fd = proc->p_fd;
		fd->fd_rdir = *(vnode**)resolve(addr_rootvnode);
		fd->fd_jdir = *(vnode**)resolve(addr_rootvnode);

        /*FTPShellCode = new OrbisShellCode();
        FTPShellCode->InstallShellCode("SceShellCore");
		pause("", 5000);
        FTPSprxHandle = FTPShellCode->sceKernelLoadStartModule("/mnt/usb0/FTP.sprx", 0, 0, 0, 0, 0);*/
		sys_proc_elf_handle(proc, (char*)OrbisFTP);
	}
	//TODO: Write Shell Code for ShellCore to do ftp and do notifies.
}
