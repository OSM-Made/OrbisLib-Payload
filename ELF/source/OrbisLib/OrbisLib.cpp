#include "../main.hpp"
#include "OrbisProc.hpp"
#include "OrbisLib.hpp"

extern uint8_t OrbisFTP[];
extern int32_t OrbisFTPSize;

void OrbisLib::OrbisLibClientThread(void* arg)
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
            orbisProc->Proc_Kill(Socket);
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

void OrbisLib::OrbisLibProcThread(void *arg) 
{
    OrbisLib* orbisLib = (OrbisLib*)arg;

    DebugLog(LOGTYPE_INFO, "Hello from kproc :)");

    //TODO: Start watcher thread to manage proc changes and handle intercepts
    //      Possibly maybe change this to call backs if we can and maybe hook the trap function

    //kproc_kthread_add(BreakMonitorThread, 0, &kDebugProc, NULL, NULL, 0, "OrbisLib3.elf", "Proc Watcher Thread");

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
			kproc_kthread_add(OrbisLibClientThread, &ClientArgs, &orbisLib->kOrbisProc, NULL, NULL, 0, "OrbisLib.elf", "Client Thread");

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
    kproc_create(OrbisLibProcThread, this, &kOrbisProc, 0, 0, "OrbisLib.elf");

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