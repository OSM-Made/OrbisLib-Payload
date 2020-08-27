#include "../main.hpp"
#include "OrbisProc.hpp"
#include "OrbisLib.hpp"

#include "../Util/Kernel.hpp"
#include "../Util/FileIO.hpp"

void OrbisLib::ClientThread(void* arg)
{
    //DebugLog(LOGTYPE_INFO, "Hello from Client Thread :)");

    ClientThreadArgs* clientThreadArgs = (ClientThreadArgs*)arg;
    OrbisLib* orbisLib = clientThreadArgs->orbisLib;
    OrbisProc* orbisProc = orbisLib->orbisProc;
    int Socket = clientThreadArgs->Socket;
    int Status = 0;

    API_Packet_s* Packet = (API_Packet_s*)_malloc(sizeof(API_Packet_s));
	memset(Packet, 0, sizeof(API_Packet_s));

    if (Receive(Socket, (char*)Packet, sizeof(API_Packet_s)))
	{
        DebugLog(LOGTYPE_INFO, "API Recieved Command %d", Packet->cmd);

        switch(Packet->cmd)
        {
        default:
            DebugLog(LOGTYPE_WARN, "Command %d is not Implemented!!", Packet->cmd);
            break;

        case API_TEST_COMMS:
            Status = 1;
            Send(Socket, (char*)&Status, sizeof(int));
            break;

        /* Proc functions */

        case API_PROC_GET_LIST:
            orbisProc->Proc_GetList(Socket);
            break;

        case API_PROC_ATTACH:
            orbisProc->Proc_Attach(Socket, Packet->ProcName);
            break;

        case API_PROC_DETACH:
            orbisProc->Proc_Detach(Socket);
            break;

        case API_PROC_GET_CURRENT:
            orbisProc->Proc_GetCurrent(Socket);
            break;

        case API_PROC_READ:
            orbisProc->Proc_Read(Socket, Packet->PROC_RW.Address, Packet->PROC_RW.len);
            break;

        case API_PROC_WRITE:
            orbisProc->Proc_Write(Socket, Packet->PROC_RW.Address, Packet->PROC_RW.len);
            break;

        case API_PROC_KILL:
            orbisProc->Proc_Kill(Socket, Packet->ProcName);
            break;
        

        /* Breakpoint functions */
        case API_BREAKPOINT_SET:
            
            break;
        
        case API_BREAKPOINT_UPDATE:

            break;

        case API_BREAKPOINT_REMOVE:

            break;

        case API_BREAKPOINT_GETINFO:

            break;

        case API_BREAKPOINT_LIST:

            break;

        /* Remote Library functions */
        case API_PROC_LOAD_SPRX:
            orbisProc->Proc_LoadSPRX(Socket, Packet->PROC_SPRX.ModuleDir, Packet->PROC_SPRX.Flags);
            break;

        case API_PROC_UNLOAD_SPRX:
            orbisProc->Proc_UnloadSPRX(Socket, Packet->PROC_SPRX.hModule, Packet->PROC_SPRX.Flags);
            break;

        case API_PROC_RELOAD_SPRX_NAME:
            orbisProc->Proc_ReloadSPRX(Socket, Packet->PROC_SPRX.ModuleDir);
            break;

        case API_PROC_RELOAD_SPRX_HANDLE:
            orbisProc->Proc_ReloadSPRX(Socket, Packet->PROC_SPRX.hModule);
            break;

        case  API_PROC_MODULE_LIST:
            orbisProc->Proc_GetModuleList(Socket);
            break;
        }
    }

    _free(Packet);
    sys_close(Socket);
    kthread_exit();
}

extern uint8_t OrbisFTP[];
extern int32_t OrbisFTPSize;

int MountNullFS(char* where, char* what, int flags)
{
    auto mount_argf = (struct mntarg*(*)(struct mntarg *ma, const char *name, const char *fmt, ...))resolve(0x001E1780);
    auto kernel_mount = (int(*)(struct mntarg	*ma, int flags))resolve(0x001E1920);

    struct mntarg* ma = NULL;

    ma = mount_argf(ma, "fstype", "%s", "nullfs");
    ma = mount_argf(ma, "fspath", "%s", where);
    ma = mount_argf(ma, "target", "%s", what);

    if (ma == NULL) {
    	DebugLog(LOGTYPE_ERR, "Something is wrong, ma value is null after argument");
    	return 50;
    }

    return kernel_mount(ma, flags);
}


void OrbisLib::ProcThread(void *arg) 
{
    OrbisLib* orbisLib = (OrbisLib*)arg;

    DebugLog(LOGTYPE_INFO, "Hello from kproc :)");

    auto vmspace_alloc = (struct vmspace* (*)(vm_offset_t min, vm_offset_t max))resolve(0x0019EB20);
	auto pmap_activate = (void(*)(struct thread *td))resolve(0x002EAFD0);

	thread* CurrentThread = curthread();
	ksetuid(0, CurrentThread);

	// Root and escape our thread
	if (CurrentThread->td_ucred)
	{
		DebugLog(LOGTYPE_INFO, "escaping thread");

		CurrentThread->td_ucred->cr_rgid = 0;
		CurrentThread->td_ucred->cr_svgid = 0;

		CurrentThread->td_ucred->cr_uid = 0;
		CurrentThread->td_ucred->cr_ruid = 0;

		if (CurrentThread->td_ucred->cr_prison)
			CurrentThread->td_ucred->cr_prison = *(prison**)resolve(addr_prison0);

		if (CurrentThread->td_proc->p_fd)
			CurrentThread->td_proc->p_fd->fd_rdir = CurrentThread->td_proc->p_fd->fd_jdir = *(vnode**)resolve(addr_rootvnode);

		// Set our auth id as debugger
		CurrentThread->td_ucred->cr_sceAuthID = 0x3800000000010003;

		// make system credentials
		CurrentThread->td_ucred->cr_sceCaps[0] = 0xFFFFFFFFFFFFFFFFULL;
		CurrentThread->td_ucred->cr_sceCaps[1] = 0xFFFFFFFFFFFFFFFFULL;

		DebugLog(LOGTYPE_INFO, "credentials rooted for new proc");
	}

	vmspace* vmspace = vmspace_alloc(0, 4096 * 2048); // Allocate 8MiB

	Log("%s", CurrentThread->td_proc->p_comm);

	CurrentThread->td_proc->p_vmspace = vmspace;
	pmap_activate(CurrentThread);

	DebugLog(LOGTYPE_INFO, "Creating initial 3 file descriptors (0, 1, 2).");
	int descriptor = sys_fopen("/dev/console", 1, 0);
	DebugLog(LOGTYPE_INFO, "/dev/console descriptor: %d", descriptor);
	DebugLog(LOGTYPE_INFO, "dup2(desc, 1) result: %d", kdup2(descriptor, 1, CurrentThread));
	DebugLog(LOGTYPE_INFO, "dup2(1, 2) result: %d", kdup2(1, 2, CurrentThread));

    auto kernel_sysctlbyname = (int(*)(thread *td, char *name, void *old, size_t *oldlenp, void *pnew, size_t newlen, size_t *retval, int flags))resolve(0x262770);
    
    size_t retval = 0;
    int32_t oldp = 0;
	size_t oldlenp = 4;
	int ret = kernel_sysctlbyname(curthread(), "kern.sdk_version", (char*)&oldp, &oldlenp, NULL, NULL, NULL, 0);
	Log("ret = %llX", ret);
	Log("SDK = %llX", oldp);

    uint64_t CR0 = __readcr0();
 	__writecr0(CR0 & ~CR0_WP);

    retval = 0;
	int32_t newp = 0x7510001;
	size_t newlenp = 4;
	ret = kernel_sysctlbyname(curthread(), "kern.sdk_version", NULL, NULL, (char*)&newp, newlenp, NULL, 0);
	Log("ret = %llX", ret);

    __writecr0(CR0);

    retval = 0;
	oldp = 0;
	oldlenp = 4;
	ret = kernel_sysctlbyname(curthread(), "kern.sdk_version", (char*)&oldp, &oldlenp, NULL, NULL, NULL, 0);
	Log("ret = %llX", ret);

	Log("NEW SDK = %llX", oldp);


	//Start up the FTP Server.
	proc* proc = proc_find_by_name("SceRemotePlay");//SceRemotePlay
	if(proc) 
	{
        //Give Root FS Perms
		ucred* cred = proc->p_ucred;
		filedesc* fd = proc->p_fd;

		prison* cr_prisonBackUp = cred->cr_prison;
		vnode* fd_jdirBackUp = fd->fd_jdir;
		vnode* fd_rdirBackUp = fd->fd_rdir;

		uint32_t r_cr_uid = cred->cr_uid;
		uint32_t r_cr_ruid = cred->cr_ruid;
		uint32_t r_cr_rgid = cred->cr_rgid;
		uint32_t r_cr_groups = cred->cr_groups[0];
		

		//mount new file System /Orbis/
		auto vn_fullpath = (int(*)(struct thread *td, struct vnode *vp, char **retbuf, char **freebuf))resolve(0xA11A0);

		char* SandboxPath = nullptr;
		char* FreePath = nullptr;
		vn_fullpath(curthread(), fd->fd_jdir, &SandboxPath, &FreePath);

		cred->cr_prison =*(prison**)resolve(addr_prison0);
		fd->fd_jdir = *(vnode**)resolve(addr_rootvnode);
		fd->fd_rdir = *(vnode**)resolve(addr_rootvnode);

		cred->cr_uid = 0;
		cred->cr_ruid = 0;
		cred->cr_rgid = 0;
		cred->cr_groups[0] = 0;

		Log("Sandbox Dir = %s\nFreePath Dir = %s", SandboxPath, FreePath);

		char AlternatePath[PATH_MAX];
		char MountPath[PATH_MAX];
		snprintf(AlternatePath, PATH_MAX, "%s/Orbis", SandboxPath);
		strcpy(MountPath, "/mnt");

		Log("AlternatePath = %s", AlternatePath);

		auto DirHandle = sys_fopen("/mnt", O_RDONLY | O_DIRECTORY, 0777);
		if(DirHandle < 0)
		{
			DebugLog(LOGTYPE_ERR, "Failed to open path %d", DirHandle);
		}

		int ret = sys_mkdir("/mnt/usb0/Orbis", 0511);
		if (ret < 0) {
			DebugLog(LOGTYPE_ERR, "Failed to make Alt Dir %d", ret);
		}	

		ret = MountNullFS(AlternatePath, MountPath, MNT_RDONLY);
		if (ret < 0) {
			DebugLog(LOGTYPE_ERR, "Failed to mount folder! %d", ret);
		}

		sys_fclose(DirHandle);

		cred->cr_prison = cr_prisonBackUp;
		//fd->fd_jdir = fd_jdirBackUp;
		//fd->fd_rdir = fd_rdirBackUp;

		cred->cr_uid = r_cr_uid;
		cred->cr_ruid = r_cr_ruid;
		cred->cr_rgid = r_cr_rgid;
		cred->cr_groups[0] = r_cr_groups;

		sys_proc_elf_handle(proc, (char*)OrbisFTP);
	}

    //TODO: Start watcher thread to manage proc changes and handle intercepts
    //      Possibly maybe change this to call backs if we can and maybe hook the trap function

    kproc_kthread_add(orbisLib->orbisProc->WatcherThread, orbisLib->orbisProc, &orbisLib->kOrbisProc, NULL, NULL, 0, "OrbisLib.elf", "Proc Watcher Thread");

    //Create a new socket for our listener.
	int ClientSocket = -1;
	int ServerSocket = sys_socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;
	sys_setsockopt(ServerSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

    //Bind and start listening on port 6900 with a back log of 100.
    sockaddr_in servaddr = { 0 };
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(6900);

    sys_bind(ServerSocket, (struct sockaddr *)&servaddr, sizeof(servaddr));
	sys_listen(ServerSocket, 100);

    DebugLog(LOGTYPE_INFO, "API Started listening on port 6900...");

    while(orbisLib->IsRunning)
    {
        kthread_suspend_check();
        
        //Wait for incoming connections.
        sockaddr_in clientaddr = { 0 };
		int addrlen = sizeof(struct sockaddr_in);
		ClientSocket = sys_accept(ServerSocket, (struct sockaddr *)&clientaddr, &addrlen);
		orbisLib->HostIPAddr = clientaddr.sin_addr.s_addr;

        if (ClientSocket != -1) 
		{
            if(orbisLib->LastHostIPAddr != orbisLib->HostIPAddr)
			    DebugLog(LOGTYPE_INFO, "New Host Connection (%i.%i.%i.%i)", clientaddr.sin_addr.s_addr & 0xFF, (clientaddr.sin_addr.s_addr >> 8) & 0xFF, (clientaddr.sin_addr.s_addr >> 16) & 0xFF, (clientaddr.sin_addr.s_addr >> 24) & 0xFF);

			int optval = 1;
			sys_setsockopt(ClientSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

            
            //Add a thread to handle the new client API Request.
            ClientThreadArgs ClientArgs = { ClientSocket, orbisLib };
			kproc_kthread_add(ClientThread, &ClientArgs, &orbisLib->kOrbisProc, NULL, NULL, 0, "OrbisLib.elf", "Client Thread");

            //Reset the temp socket for the next connection.
			ClientSocket = -1;

            //Set our last Connection so we can tell when we have a new host.
            orbisLib->LastHostIPAddr = orbisLib->HostIPAddr;
		}
    }

    //kthread_exit();
    DebugLog(LOGTYPE_NONE, "kproc Exiting!!!");
	sys_close(ServerSocket);
    kproc_exit(0);
}

OrbisLib::OrbisLib()
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");

    //Safety bool to make sure the threads shut down gracefully when needed.
    IsRunning = true;

    //Create OrbisProc class and make sure it has allocated - helper class for API commands for proc's.
    orbisProc = new OrbisProc();
    if(orbisProc == NULL)
    {
        DebugLog(LOGTYPE_ERR, "Failed to allocate orbisProc class!!");
        IsRunning = false;
        return;
    }

    //Create Proc used to run the API and debugging.
    kproc_create(ProcThread, this, &kOrbisProc, 0, 0, "OrbisLib.elf");

    //Make sure the proc was made.
    if(!kOrbisProc)
    {
        DebugLog(LOGTYPE_ERR, "Failed to create OrbisProc!!");
        IsRunning = false;
        return;
    }

    //Set our proc titleID doesnt really do anything is just cool :)
    strcpy(kOrbisProc->titleId, "OSML10000");
}

OrbisLib::~OrbisLib()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");
    
    //Signal threads to shutdown.
    IsRunning = false;

    //Kill the DebugLogger
    StopDebugLogger();

    //Free the OrbisProc Class
    delete orbisProc;
}