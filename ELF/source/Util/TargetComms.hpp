#pragma once
#define PORT_TARGETSERVER 6901

extern "C"
{
    #include <machine/reg.h>
}

enum TargetCommands
{
	CMD_PRINT,

	CMD_INTERCEPT,
	CMD_CONTINUE,

	CMD_PROC_DIE,
	CMD_PROC_ATTACH,
	CMD_PROC_DETACH,

	CMD_TARGET_SUSPEND,
    CMD_TARGET_RESUME,
    CMD_TARGET_SHUTDOWN,
};

enum PrintType
{
	PT_SOCK,
	PT_SERIAL
};

struct TargetCommandPacket_s
{
	int CommandIndex;
	union
	{
		char ProcName[0x20];
		struct
		{
			int Type;
			int Len;
		}Print;
		struct
		{
			int Reason;
			reg Registers;
		}Break;
	};
};

int SendTargetCommand(int Command, TargetCommandPacket_s* TargetCommandPacket);
void SendTargetCommand(int Command);

void SendProcChange(char* ProcName);
void SendPrint(char* Data, int len);
void SendIntercept(int Reason, reg* Registers);