#include "../main.hpp"
#include "Debug-Logger.hpp"

//[Type][Function:Line] %s
//TODO: Refactor to use buffer to write to socket on another thread

int DebugMode = 0;
uint32_t DebugIP = 0;
#define OSM
void printf(char* fmt, ...) {
	//if(DebugMode == 0)
	//	return; 

	char buffer[0x400] = { 0 };

	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	int sock = sys_socket(AF_INET, SOCK_STREAM, 0);

	int optval = 1;
	sys_setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&optval, sizeof(int));

	struct sockaddr_in sockAddr = { 0 };
	sockAddr.sin_family = AF_INET;
	#ifdef OSM
	sockAddr.sin_port = 44582; //29975
	sockAddr.sin_addr.s_addr = IP(192, 168, 1, 166);
	#else
	sockAddr.sin_port = 29975; //
	sockAddr.sin_addr.s_addr = 0x7F8EC20A;
	#endif
	//sockAddr.sin_addr.s_addr = DebugIP;

	sys_connect(sock, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr));
	sys_write(sock, buffer, 0x400);
	sys_close(sock);

	va_end(args);
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
		printf("%s\n", buffer);
	else
		printf("[%s][%s:%d] %s\n", Prefix, FunctionName, LineNumber, buffer);

	va_end(args);
}