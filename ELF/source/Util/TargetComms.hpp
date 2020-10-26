#pragma once
#define PORT_TARGETSERVER 6901

extern "C"
{
    #include <machine/reg.h>
}

enum TargetCommands
{
	//Print Server
	CMD_PRINT = 1,

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

#pragma pack(push, 2)
struct TargetCommandPacket_s
{
	int CommandIndex; //0x00
	char IPAddr[16]; //0x04
	union
	{
		char ProcName[0x20];
		struct
		{
			char TitleID[0x20];
		}TitleChange;
		struct
		{
			char Sender[0x100]; //0x14
			char Data[0x400]; //0x114
			int Type;
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
#pragma pack(pop)

struct TargetCommBackLog_s
{
	bool Used;
	bool ReadyToSend;
	TargetCommandPacket_s TargetCommandPacket;
};

class TargetComms
{
private:
	mtx mLock;
	bool IsRunning;

	bool SendTargetCommand(TargetCommandPacket_s* TargetCommandPacket);
	int GetFreeBackLog();
	static void TargetCommsThread(void* arg);

public:
	void SendTargetCommand(int Command);
	void SendProcChange(char* ProcName);
	void SendNewTitle(char* TitleID);
	void SendPrint(char* Sender, int Type, const char* fmt, ...);
	void SendIntercept(int Reason, reg* Registers);

	void StartUpThread();
	TargetComms();
	~TargetComms();
};