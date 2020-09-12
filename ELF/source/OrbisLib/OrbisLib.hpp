#pragma once

#include "OrbisProc.hpp"
#include "OrbisTarget.hpp"
#include "OrbisDebugger.hpp"
#include "OrbisDef.hpp"

class OrbisLib
{
private:
    bool IsRunning = false;
<<<<<<< HEAD
=======
    OrbisProc* orbisProc;
    OrbisTarget* orbisTarget;
    OrbisDebugger* orbisDebugger;
>>>>>>> eb836106f5511afd9275113850f373a6d0bf0a03
    int LastHostIPAddr;

    struct ClientThreadArgs
    {
        int Socket;
        OrbisLib* orbisLib;
    };

    static void ClientThread(void* arg);
    static void ProcThread(void *arg);

public:
<<<<<<< HEAD
    OrbisProc* orbisProc;
    OrbisTarget* orbisTarget;
    OrbisDebugger* orbisDebugger;

=======
>>>>>>> eb836106f5511afd9275113850f373a6d0bf0a03
    proc* kOrbisProc = 0;
    int HostIPAddr;

    OrbisLib();
    ~OrbisLib();
};
