#include "../main.hpp"
#include "DebugLogger.hpp"
#include "Network.hpp"

LogBuffer_s LogBuffer[100];
bool ShouldDebugLoggerRun = false;
mtx mLock;

void printf(char* fmt, ...) 
{	
	char buffer[0x400] = { 0 };
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	//Prepare a new socket for our print
	int sock = sys_socket(AF_INET, SOCK_STREAM, 0);

	//Set the Socket Option SO_NOSIGPIPE
	int optval = 1;
	sys_setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

	//Connect to our host on the port 9902
	struct sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(9902);
	sockAddr.sin_addr.s_addr = IP(192, 168, 1, 64);

	sys_connect(sock, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr));

	//Send our current buffer to the host.
	sys_write(sock, buffer, sizeof(buffer));

	//Close the Socket
	sys_close(sock);

	va_end(args);
}

void WriteBuffer(const char* fmt, ...)
{
	char buffer[0x400] = { 0 };
	int BufferIndex = 99;
	bool BufferFound = false;

	//mtx_lock_flags(&mLock, 0);

	//Get index of free buffer space.
	do
	{
		if (!LogBuffer[BufferIndex].BufferUsed)
		{
			LogBuffer[BufferIndex].BufferUsed = true;
			BufferFound = true;
		}
		else
			BufferIndex --;
	} while (BufferFound == false && BufferIndex != -1);

	//Make sure we found our Buffer.
	if (BufferIndex == -1 || BufferFound == false)
	{

		return;
	}

	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	//Copy our buffer to the back log and set it to send.
	memcpy(&LogBuffer[BufferIndex].Buffer[0], buffer, sizeof(LogBuffer[BufferIndex].Buffer));
	LogBuffer[BufferIndex].BufferReady = true;

	va_end(args);

	//mtx_unlock_flags(&mLock, 0);
}

void LogInternal(enum LogType_e LogType, const char* FunctionName, int32_t LineNumber, const char* fmt, ...)
{
	const char* Prefix = "";
	switch(LogType)
	{
		case LOGTYPE_NONE:
			break;
		case LOGTYPE_INFO:
			Prefix = "INFO";
			break;

		case LOGTYPE_WARN:
			Prefix = "WARNING";
			break;

		case LOGTYPE_ERR:
			Prefix = "ERROR";
			break;
	}

	char buffer[0x400] = { 0 };

	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	if(LogType == LOGTYPE_NONE)
		WriteBuffer("%s\n", buffer);
	else
		WriteBuffer("[%s][%s:%d] %s\n", Prefix, FunctionName, LineNumber, buffer);

	va_end(args);
}

void SendBuffer(char* Buffer) 
{	
	//Prepare a new socket for our print
	int sock = sys_socket(AF_INET, SOCK_STREAM, 0);

	//Set the Socket Option SO_NOSIGPIPE
	int optval = 1;
	sys_setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

	//Connect to our host on the port 9902
	struct sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(9902);
	sockAddr.sin_addr.s_addr = orbisLib->HostIPAddr;

	sys_connect(sock, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr));

	//Send our current buffer to the host.
	sys_write(sock, Buffer, 0x400);

	//Close the Socket
	sys_close(sock);
}

void DebugLoggerThread(void* arg)
{
	//DebugLog(LOGTYPE_INFO, "Waiting For Host Connection...");

	//Wait for our host to connect
	while(orbisLib->HostIPAddr == 0)
	{
		pause("", 100);
	}

	//DebugLog(LOGTYPE_INFO, "New Client Connection!");

	while(ShouldDebugLoggerRun)
	{
		for(int i = 100; i > 0; i--)
		{
			if(LogBuffer[i].BufferReady)
			{
				SendBuffer(LogBuffer[i].Buffer);
				memset(&LogBuffer[i].Buffer[0], 0, 0x400);
				LogBuffer[i].BufferReady = false;
				LogBuffer[i].BufferUsed = false;
			}
		}
		pause("", 100);
	}

	//DebugLog(LOGTYPE_INFO, "Debug Logger Thread Shutting Down!");
	kthread_exit();
}

void StartDebugLogger()
{
	DebugLog(LOGTYPE_INFO, "Debug Logger Start up!!");

	ShouldDebugLoggerRun = true;

	//TODO:Remove after testing...
	orbisLib->HostIPAddr = IP(192, 168, 1, 64);

	while(orbisLib->kOrbisProc == 0)
		pause("", 100);

	kproc_kthread_add(DebugLoggerThread, 0, &orbisLib->kOrbisProc, NULL, NULL, 0, "OrbisLib.elf", "Debug Logger Thread");

	mtx_init(&mLock, "DebugloggermLock", 0, MTX_DEF);
}

void StopDebugLogger()
{
	DebugLog(LOGTYPE_INFO, "Debug Logger Shutdown!!");

	ShouldDebugLoggerRun = false;

	mtx_destroy(&mLock);
}