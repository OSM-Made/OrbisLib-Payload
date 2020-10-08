#pragma once

#include "OrbisShellCode.hpp"

class OrbisProc
{
private:
    bool IsRunning = false;
    int CurrentProcessID = -1;
    OrbisShellCode* orbisShellCode;
    
    //Event Handlers
    eventhandler_entry* ProcessStartEvent;
    eventhandler_entry* ProcessExitEvent;

public:
    char CurrentProcName[0x20] = { };
    bool CurrentlyAttached = false;

    OrbisProc();
    ~OrbisProc();
    
    static void OnProcessStart(void *arg, struct proc *p);
    static void OnProcessExit(void *arg, struct proc *p);

    int API_CallSetup(int Socket, proc** proc);

    void Proc_GetList(int Socket);
    void Proc_Attach(int Socket, char* ProcName);
    void Proc_Detach(int Socket);
    void Proc_GetCurrent(int Socket);
    void Proc_Read(int Socket, uint64_t Address, size_t len);
    void Proc_Write(int Socket, uint64_t Address, size_t len);
    void Proc_Kill(int Socket, char* ProcName);

    void Proc_LoadSPRX(int Socket, const char *name, unsigned int flags);
    void Proc_UnloadSPRX(int Socket, int handle, uint32_t flags);
    void Proc_ReloadSPRX(int Socket, const char *name);
    void Proc_ReloadSPRX(int Socket, int Handle);
    void Proc_GetModuleList(int Socket);

    void APIHandle(int Socket, API_Packet_s* Packet);
};