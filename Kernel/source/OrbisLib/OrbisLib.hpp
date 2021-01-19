#pragma once

#include "OrbisProc.hpp"
#include "OrbisTarget.hpp"
#include "OrbisDebugger.hpp"
#include "OrbisDef.hpp"

class OrbisLib
{
private:
    bool IsRunning = false;
    int LastHostIPAddr;

    //Event Handlers
    eventhandler_entry* SystemSuspend;
    eventhandler_entry* SystemResume;
    eventhandler_entry* SystemShutdown;

    struct ClientThreadArgs
    {
        int Socket;
        OrbisLib* orbisLib;
    };

    static void ClientThread(void* arg);
    static void ProcThread(void *arg);

    static void OnSystemSuspend(void *arg);
    static void OnSystemResume(void *arg);
    static void OnSystemShutdown(void *arg);

public:
    OrbisProc* orbisProc;
    OrbisTarget* orbisTarget;
    OrbisDebugger* orbisDebugger;

    proc* kOrbisProc = 0;
    int HostIPAddr;

    OrbisLib();
    ~OrbisLib();
};
