#pragma once
#define PORT_TARGETSERVER 6901

extern "C"
{
    #include <machine/reg.h>
}

enum TargetCommands
{
	//Print Server
	CMD_PRINT,

	//Debugging
	CMD_INTERCEPT,
	CMD_CONTINUE,

	//Proc States
	CMD_PROC_DIE,
	CMD_PROC_ATTACH,
	CMD_PROC_DETACH,

	//Target State
	CMD_TARGET_SUSPEND,
	CMD_TARGET_RESUME,
	CMD_TARGET_SHUTDOWN,
	CMD_TARGET_NEWTITLE,

	//DB Watcher
	CMD_DB_TOUCHED,

	//Target Availability
	CMD_TARGET_AVAILABILITY,
};

enum PrintType
{
	PT_SOCK,
	PT_SERIAL
};

struct TargetCommandPacket_s
{
	int CommandIndex;
	char IPAddr[16];
	union
	{
		char ProcName[0x20];
		struct
		{
			char TitleID[0x20];
		}TitleChange;
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
		struct
		{
			bool Available;
			char TargetName[0x100];
		}Target;
	};
};

int SendTargetCommand(int Command, TargetCommandPacket_s* TargetCommandPacket);
void SendTargetCommand(int Command);

void SendProcChange(char* ProcName);
void SendNewTitle(char* TitleID);
void SendPrint(char* Data, int len);
void SendIntercept(int Reason, reg* Registers);