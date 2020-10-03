#include "../Main.hpp"
#include "TargetComms.hpp"

int SendTargetCommand(int Command, TargetCommandPacket_s* TargetCommandPacket)
{
    //Prepare our buffer
    TargetCommandPacket->CommandIndex = Command;

    //Prepare a new socket for our print
	int sock = sys_socket(AF_INET, SOCK_STREAM, 0);

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
	Send(sock, (char*)TargetCommandPacket, sizeof(TargetCommandPacket_s));

	return sock;
}

void SendTargetCommand(int Command)
{
    //Prepare our packet.
    TargetCommandPacket_s TargetCommandPacket;

    //Send the command packet.
    int sock = SendTargetCommand(Command, &TargetCommandPacket);

    //close the socket.
    sys_close(sock);
}

void SendProcChange(char* ProcName)
{
    //Prepare our packet.
    TargetCommandPacket_s TargetCommandPacket;
    strcpy(TargetCommandPacket.ProcName, ProcName);

    //send the command packet.
    int sock = SendTargetCommand(CMD_PROC_ATTACH, &TargetCommandPacket);

    //Close the socket.
    sys_close(sock);
}

void SendPrint(char* Data, int len)
{
    //Prepare our packet.
    TargetCommandPacket_s TargetCommandPacket;
    TargetCommandPacket.Print.Type = PT_SOCK;
    TargetCommandPacket.Print.Len = len;

    //send the command packet.
    int sock = SendTargetCommand(CMD_PRINT, &TargetCommandPacket);

    //Send the data from the print.
    Send(sock, Data, len);

    //Close the socket.
    sys_close(sock);
}

void SendIntercept(int Reason, reg* Registers)
{
    //Prepare our packet.
    TargetCommandPacket_s TargetCommandPacket;
    TargetCommandPacket.Break.Reason = Reason;
    memcpy((void*)&TargetCommandPacket.Break.Registers, Registers, sizeof(reg));

    //send the command packet.
    int sock = SendTargetCommand(CMD_INTERCEPT, &TargetCommandPacket);

    //Close the socket.
    sys_close(sock);
}