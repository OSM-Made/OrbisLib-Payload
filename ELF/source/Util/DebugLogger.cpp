#include "../main.hpp"
#include "DebugLogger.hpp"
#include "Network.hpp"

void printf(char* fmt, ...) 
{	
	char buffer[0x400] = { "TEST" };
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

void LogInternal(enum LogType_e LogType, const char* FunctionName, int32_t LineNumber, const char* fmt, ...)
{
	char buffer[0x400] = { 0 };

	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);

	if(LogType == LOGTYPE_NONE){

		//printf("%s\n", buffer);
		pTargetComms->SendPrint("OrbisLib", LogType, "%s\n", buffer);
	}
	else
	{
		//printf("[%s:%d] %s\n", FunctionName, LineNumber, buffer);
		pTargetComms->SendPrint("OrbisLib", LogType, "[%s:%d] %s\n", FunctionName, LineNumber, buffer);
	}
		

	va_end(args);
}