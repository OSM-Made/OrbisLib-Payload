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
};