#pragma once

void printf(char* fmt, ...);

enum LogType_e
{
    LOGTYPE_NONE,
    LOGTYPE_INFO,
    LOGTYPE_WARN,
    LOGTYPE_ERR,
};

void LogInternal(enum LogType_e LogType, const char* FunctionName, int32_t LineNumber, const char* fmt, ...);
#define DebugLog(LogType, fmt, ...) LogInternal(LogType, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define Log(fmt, ...) LogInternal(LOGTYPE_NONE, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)