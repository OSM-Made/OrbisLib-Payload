#pragma once

class OrbisProc
{
private:
    char CurrentProc[0x20] = { };
    bool CurrentlyAttached = false;
    int CurrentProcessID = -1;

    struct RESP_ProcList
    {
        unsigned int ProcessID; //0x00
        unsigned int Attached; //0x04
        char ProcName[32]; //0x08
        char TitleID[10]; //0x28
    };

public:
    OrbisProc();
    ~OrbisProc();

    void Proc_GetList(int Socket);
    void Proc_Attach(int Socket, char* ProcName);
    void Proc_Detach(int Socket);
    void Proc_Read(int Socket, uint64_t Address, size_t len);
    void Proc_Write(int Socket, uint64_t Address, size_t len);
    
};