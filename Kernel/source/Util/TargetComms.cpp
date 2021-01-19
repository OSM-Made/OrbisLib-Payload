#include "../Main.hpp"
#include "TargetComms.hpp"

#define TARGETCOMM_BACKLOG 100
TargetCommBackLog_s TargetCommBackLog[TARGETCOMM_BACKLOG];

bool TargetComms::SendTargetCommand(TargetCommandPacket_s* TargetCommandPacket)
{
    int sock = -1;

    //Prepare a new socket for our print
	sock = sys_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Set the Socket Option SO_NOSIGPIPE
	int optval = 1;
	sys_setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

	//Connect to our host on the port 6901
	struct sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(PORT_TARGETSERVER);
	sockAddr.sin_addr.s_addr = orbisLib->HostIPAddr;

    //Connect to Host.
	sys_connect(sock, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr));

    //Send our command packet.
    Send(sock, (char*)TargetCommandPacket, sizeof(TargetCommandPacket_s)); //sys_write

	//Close the socket.
    sys_close(sock);

    return true;
}

int TargetComms::GetFreeBackLog()
{
    int IndexFound = TARGETCOMM_BACKLOG - 1;
    bool SlotFound = false;

    do
	{
		if (!TargetCommBackLog[IndexFound].Used)
        {
            //Set as used.
            TargetCommBackLog[IndexFound].Used = true;

			SlotFound = true;
        }
		else
			IndexFound --;
	} while (SlotFound == false && IndexFound != -1);

    //Make sure we found our Buffer.
	if (IndexFound == -1 || SlotFound == false)
		return -1;

    return IndexFound;
}

void TargetComms::SendTargetCommand(int Command)
{
    //We must lock before we make changes so we dont send incomplete packets.
    //mtx_lock_flags(&mLock, 0);

    //Add Our command to the Que.
    int index = GetFreeBackLog();

    //Make sure the index was found.
    if(index == -1)
    {
        mtx_unlock_flags(&mLock, 0);
        return;
    }
    
    //Copy our data to the packet.
    TargetCommBackLog[index].TargetCommandPacket.CommandIndex = Command;

    //Set Packet to send.
    TargetCommBackLog[index].ReadyToSend = true;

    //dont forget to unlock.
    //mtx_unlock_flags(&mLock, 0);
}

void TargetComms::SendProcChange(char* ProcName)
{
    //We must lock before we make changes so we dont send incomplete packets.
    //mtx_lock_flags(&mLock, 0);

    //Add Our command to the Que.
    int index = GetFreeBackLog();

    //Make sure the index was found.
    if(index == -1)
    {
        mtx_unlock_flags(&mLock, 0);
        return;
    }

    //Copy our data to the packet.
    TargetCommBackLog[index].TargetCommandPacket.CommandIndex = CMD_PROC_ATTACH;
    strcpy(TargetCommBackLog[index].TargetCommandPacket.ProcName , ProcName);

    //Set Packet to send.
    TargetCommBackLog[index].ReadyToSend = true;

    //dont forget to unlock.
    //mtx_unlock_flags(&mLock, 0);
}

void TargetComms::SendNewTitle(char* TitleID)
{
    //We must lock before we make changes so we dont send incomplete packets.
    //mtx_lock_flags(&mLock, 0);

    //Add Our command to the Que.
    int index = GetFreeBackLog();

    //Make sure the index was found.
    if(index == -1)
    {
        mtx_unlock_flags(&mLock, 0);
        return;
    }

    //Copy our data to the packet.
    TargetCommBackLog[index].TargetCommandPacket.CommandIndex = CMD_TARGET_NEWTITLE;
    strcpy(TargetCommBackLog[index].TargetCommandPacket.TitleChange.TitleID, TitleID);

    //Set Packet to send.
    TargetCommBackLog[index].ReadyToSend = true;

    //dont forget to unlock.
    //mtx_unlock_flags(&mLock, 0);
}

void TargetComms::SendPrint(char* Sender, int Type, const char* fmt, ...)
{
    char Data[0x400] = { 0 };

    va_list args;
	va_start(args, fmt);
	vsprintf(Data, fmt, args);
    va_end(args);

    //We must lock before we make changes so we dont send incomplete packets.
    //mtx_lock_flags(&mLock, 0);

    //Add Our command to the Que.
    int index = GetFreeBackLog();

    //Make sure the index was found.
    if(index == -1)
    {
        mtx_unlock_flags(&mLock, 0);
        return;
    }
    
    //Copy our data to the packet.
    TargetCommBackLog[index].TargetCommandPacket.CommandIndex = CMD_PRINT;
    strcpy(TargetCommBackLog[index].TargetCommandPacket.Print.Sender, Sender);
    TargetCommBackLog[index].TargetCommandPacket.Print.Type = Type;
    memcpy(&TargetCommBackLog[index].TargetCommandPacket.Print.Data[0], Data, 0x400);

    //Set Packet to send.
    TargetCommBackLog[index].ReadyToSend = true;

    //dont forget to unlock.
    //mtx_unlock_flags(&mLock, 0);
}

void TargetComms::SendIntercept(int Reason, reg* Registers)
{
    //We must lock before we make changes so we dont send incomplete packets.
    //mtx_lock_flags(&mLock, 0);

    //Add Our command to the Que.
    int index = GetFreeBackLog();

    //Make sure the index was found.
    if(index == -1)
    {
        mtx_unlock_flags(&mLock, 0);
        return;
    }

    //Copy our data to the packet.
    TargetCommBackLog[index].TargetCommandPacket.CommandIndex = CMD_INTERCEPT;
    TargetCommBackLog[index].TargetCommandPacket.Break.Reason = Reason;
    memcpy((void*)&TargetCommBackLog[index].TargetCommandPacket.Break.Registers, Registers, sizeof(reg));

    //Set Packet to send.
    TargetCommBackLog[index].ReadyToSend = true;

    //dont forget to unlock.
    //mtx_unlock_flags(&mLock, 0);
}

void TargetComms::TargetCommsThread(void* arg)
{
    TargetComms* targetComms = (TargetComms*)arg;

    //Wait for our host to connect
	while(orbisLib->HostIPAddr == 0)
	{
		pause("", 100);
	}

    while(targetComms->IsRunning)
    {
        kthread_suspend_check();

        int i = TARGETCOMM_BACKLOG - 1;
        do
        {
            //lock the mutex when sending.
            //mtx_lock_flags(&targetComms->mLock, 0);

            if(TargetCommBackLog[i].Used && TargetCommBackLog[i].ReadyToSend)
            {
                if(targetComms->SendTargetCommand(&TargetCommBackLog[i].TargetCommandPacket))
                {
                    //Set the status to free.
                    memset(&TargetCommBackLog[i].TargetCommandPacket, 0, sizeof(TargetCommandPacket_s));
                    TargetCommBackLog[i].Used = false;
                    TargetCommBackLog[i].ReadyToSend = false;
                }
            }

            //dont forget to unlock.
            //mtx_unlock_flags(&targetComms->mLock, 0);

            i--;
        } while (i >= 0);
        
        //pause("", 100);
    }

    //Clean up
    //mtx_destroy(&targetComms->mLock);

    //shut down.
    kthread_exit();
}

void TargetComms::StartUpThread()
{
    while(orbisLib->kOrbisProc == 0)
		pause("", 100);

    kproc_kthread_add(TargetCommsThread, this, &orbisLib->kOrbisProc, NULL, NULL, 0, "OrbisLib.elf", "Target Comms Thread");
}

TargetComms::TargetComms()
{
    //initialize the mutex.
    //mtx_init(&this->mLock, "TargetComms_mLock", 0, MTX_DEF);

    IsRunning = true;

    for(int i = 0; i < TARGETCOMM_BACKLOG; i++)
    {
        memset(&TargetCommBackLog[i].TargetCommandPacket, 0, sizeof(TargetCommandPacket_s));
        TargetCommBackLog[i].Used = false;
        TargetCommBackLog[i].ReadyToSend = false;
    }
}

TargetComms::~TargetComms()
{
    IsRunning = false;
}