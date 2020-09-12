#pragma once

#include "OrbisShellCode.hpp"

class OrbisProc
{
private:
    bool IsRunning = false;
<<<<<<< HEAD
=======
    char CurrentProcName[0x20] = { };
    bool CurrentlyAttached = false;
>>>>>>> eb836106f5511afd9275113850f373a6d0bf0a03
    int CurrentProcessID = -1;
    OrbisShellCode* orbisShellCode;
    
    //Event Handlers
    eventhandler_entry* ProcessExitEvent;

public:
<<<<<<< HEAD
    char CurrentProcName[0x20] = { };
    bool CurrentlyAttached = false;

    OrbisProc();
    ~OrbisProc();
    
    static void OnProcessExit(void *arg, struct proc *p);
=======
    OrbisProc();
    ~OrbisProc();

    static void OnProcessExit(void *arg, struct proc *p);
    static void WatcherThread(void* arg);
>>>>>>> eb836106f5511afd9275113850f373a6d0bf0a03

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