#pragma once

enum LogType_e
{
    LOGTYPE_NONE,
    LOGTYPE_INFO,
    LOGTYPE_WARN,
    LOGTYPE_ERR,
};

struct LogBuffer_s
{
    bool BufferUsed;
	bool BufferReady;
	char Buffer[0x400];
};

void printf(char* fmt, ...) ;
void LogInternal(enum LogType_e LogType, const char* FunctionName, int32_t LineNumber, const char* fmt, ...);
#define DebugLog(LogType, fmt, ...) LogInternal(LogType, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define Log(fmt, ...) LogInternal(LOGTYPE_NONE, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);