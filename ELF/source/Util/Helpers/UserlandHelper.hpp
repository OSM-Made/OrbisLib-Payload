#pragma once

class UserlandHelper
{
private:
    char ProcName[0x20] = { };
    int ProcessID = 0;

    void* gShellCodePtr = NULL;
    void* gStackPtr = NULL;
    size_t gShellCodeSize = 0;
    bool ShellCodeLoaded = false;

    struct OrbisUserlandHelper_header
    {
        uint32_t magic;
	    uint64_t entry;

        uint64_t thr_initial;
        uint8_t ShellCodeComplete;
	    uint8_t CommandIndex;
        uint8_t ShouldExit;

        uint32_t MessageType;
        char Message[100];
    }__attribute__((packed));

    enum ShellCodeCommands
    {
        CMD_NA = 0,
        CMD_sceSysUtilSendNotificationRequest,
        CMD_sceSysUtilSendSystemNotificationWithText
    };

public:
    UserlandHelper(/* args */);
    ~UserlandHelper();

    void InstallShellCode(char* ProcName);
    void DestroyShellCode();
    void sceSysUtilSendNotificationRequest(const char* Message);
    void sceSysUtilSendSystemNotificationWithText(int MessageType, const char* Message);

};
