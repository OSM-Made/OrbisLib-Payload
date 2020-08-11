#pragma once
#include "OrbisShellCode.hpp"

class OrbisProc
{
private:
    char CurrentProcName[0x20] = { };
    bool CurrentlyAttached = false;
    int CurrentProcessID = -1;
    OrbisShellCode* orbisShellCode;

    struct RESP_ProcList
    {
        unsigned int ProcessID; //0x00
        unsigned int Attached; //0x04
        char ProcName[32]; //0x08
        char TitleID[10]; //0x28
    };

    struct RESP_CurrentProc
    {
        unsigned int ProcessID; //0x00
        char ProcName[32]; //0x04
        char TitleID[10]; //0x24
    };

    struct RESP_ModuleList
    {
        char mName[0x24]; //0x00
        char mPath[0x100]; //0x24
        int mHandle; //0x124
        uint64_t mTextSegmentBase; //0x128
        uint64_t mTextSegmentLen; //0x130
        uint64_t mDataSegmentBase; //0x138
        uint64_t mDataSegmentLen; //0x140
    };

public:
    OrbisProc();
    ~OrbisProc();

    void Proc_GetList(int Socket);
    void Proc_Attach(int Socket, char* ProcName);
    void Proc_Detach(int Socket);
    void Proc_GetCurrent(int Socket);
    void Proc_Read(int Socket, uint64_t Address, size_t len);
    void Proc_Write(int Socket, uint64_t Address, size_t len);
    void Proc_Kill(int Socket);

    void Proc_LoadSPRX(int Socket, const char *name, unsigned int flags);
    void Proc_UnloadSPRX(int Socket, int handle, uint32_t flags);
    void Proc_ReloadSPRX(int Socket, const char *name);
    void Proc_ReloadSPRX(int Socket, int Handle);
    void Proc_GetModuleList(int Socket);
};