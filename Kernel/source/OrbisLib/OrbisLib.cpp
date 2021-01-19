#include "../main.hpp"
#include "OrbisProc.hpp"
#include "OrbisTarget.hpp"
#include "OrbisDebugger.hpp"
#include "OrbisLib.hpp"
#include "OrbisDef.hpp"

#include "../Util/Kernel.hpp"
#include "../Util/FileIO.hpp"

void OrbisLib::ClientThread(void* arg)
{
    ClientThreadArgs* clientThreadArgs = (ClientThreadArgs*)arg;
    OrbisLib* orbisLib = clientThreadArgs->orbisLib;
    OrbisProc* orbisProc = orbisLib->orbisProc;
    OrbisDebugger* orbisDebugger = orbisLib->orbisDebugger;
    OrbisTarget* orbisTarget = orbisLib->orbisTarget;
    int Socket = clientThreadArgs->Socket;

    API_Packet_s* Packet = (API_Packet_s*)_malloc(sizeof(API_Packet_s));
	memset(Packet, 0, sizeof(API_Packet_s));

    if (Receive(Socket, (char*)Packet, sizeof(API_Packet_s)))
	{
        //Make sure we got a valid command index.
        if(Packet->cmd <= NULL_PACKET || Packet->cmd > TARGET_END)
        {
            DebugLog(LOGTYPE_WARN, "Invalid Packet!!!");

            goto ClientThreadEnd;
        }

        if(Packet->cmd != API_TEST_COMMS && Packet->cmd != API_TARGET_INFO)
            DebugLog(LOGTYPE_INFO, "API Recieved Command %s(%d)", API_COMMANDS_STR[Packet->cmd], Packet->cmd);

        //Direct our API call to our specified class.
        switch(Packet->cmd)
        {
        default:
            DebugLog(LOGTYPE_WARN, "Command %d is not Implemented!!", Packet->cmd);
            break;

        case API_TEST_COMMS:
            SendStatus(Socket, API_OK);
            break;

        case PROC_START ... PROC_END:
            orbisProc->APIHandle(Socket, Packet);
            break;

        case DBG_START ... DBG_END:
            orbisDebugger->APIHandle(Socket, Packet);
            break;

        case KERN_START ... KERN_END:
            DebugLog(LOGTYPE_WARN, "Not Implimented!");
	        SendStatus(Socket, API_ERROR_FAIL);
            break;

        case TARGET_START ... TARGET_END:
            orbisTarget->APIHandle(Socket, Packet);
            break;
        }
    }

ClientThreadEnd:
    _free(Packet);
    sys_close(Socket);
    kthread_exit();
}

void OrbisLib::ProcThread(void *arg) 
{
    OrbisLib* orbisLib = (OrbisLib*)arg;

	thread* CurrentThread = curthread();
	ksetuid(0, CurrentThread);

	// Root and escape our thread
	if (CurrentThread->td_ucred)
	{
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
	}

    //TODO: Start watcher thread to manage proc changes and handle intercepts
    //      Possibly maybe change this to call backs if we can and maybe hook the trap function

    kproc_kthread_add(orbisLib->orbisDebugger->WatcherThread, orbisLib, &orbisLib->kOrbisProc, NULL, NULL, 0, "OrbisLib.elf", "Proc Watcher Thread");

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

void OrbisLib::OnSystemSuspend(void *arg)
{
    pTargetComms->SendTargetCommand(CMD_TARGET_SUSPEND);

}

void OrbisLib::OnSystemResume(void *arg)
{
    pTargetComms->SendTargetCommand(CMD_TARGET_RESUME);
}

void OrbisLib::OnSystemShutdown(void *arg)
{
    pTargetComms->SendTargetCommand(CMD_TARGET_SHUTDOWN);
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

    orbisTarget = new OrbisTarget();
    if(orbisTarget == NULL)
    {
        DebugLog(LOGTYPE_ERR, "Failed to allocate orbisTarget class!!");
        IsRunning = false;
        return;
    }

    orbisDebugger = new OrbisDebugger(orbisProc);
    if(orbisDebugger == NULL)
    {
        DebugLog(LOGTYPE_ERR, "Failed to allocate orbisDebugger class!!");
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
    strcpy(kOrbisProc->titleId, "ORBS20000");

    //Register Callbacks.
    SystemSuspend = EVENTHANDLER_REGISTER(system_suspend_phase1, (void*)OnSystemSuspend, this, EVENTHANDLER_PRI_FIRST);
    SystemResume = EVENTHANDLER_REGISTER(system_resume_phase1, (void*)OnSystemResume, this, EVENTHANDLER_PRI_LAST);
    SystemShutdown = EVENTHANDLER_REGISTER(shutdown_pre_sync, (void*)OnSystemShutdown, this, EVENTHANDLER_PRI_FIRST);
}

OrbisLib::~OrbisLib()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");
    
    //Signal threads to shutdown.
    IsRunning = false;

    //Free the OrbisProc Class
    delete orbisProc;

    //Unregister Callbacks.
    EVENTHANDLER_DEREGISTER(system_suspend_phase1, SystemSuspend);
    EVENTHANDLER_DEREGISTER(system_resume_phase1, SystemResume);
    EVENTHANDLER_DEREGISTER(shutdown_pre_sync, SystemShutdown);    
}