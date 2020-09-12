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

    struct ClientThreadArgs
    {
        int Socket;
        OrbisLib* orbisLib;
    };

    static void ClientThread(void* arg);
    static void ProcThread(void *arg);

public:
    OrbisProc* orbisProc;
    OrbisTarget* orbisTarget;
    OrbisDebugger* orbisDebugger;

    proc* kOrbisProc = 0;
    int HostIPAddr;

    OrbisLib();
    ~OrbisLib();
};
