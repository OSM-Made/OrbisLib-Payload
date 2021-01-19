#pragma once

#include "OrbisBreakPoint.hpp"
#include "OrbisWatchPoint.hpp"

#define BREAKPOINTS_MAX 10
#define WATCHPOINTS_MAX 4

struct trapframe_s {
	register_t	tf_rdi;
	register_t	tf_rsi;
	register_t	tf_rdx;
	register_t	tf_rflags;
	register_t	tf_rsp;
	register_t	tf_ss;
};

class OrbisDebugger
{
private:
    OrbisProc* orbisProc;
    OrbisBreakPoint* Breakpoints[BREAKPOINTS_MAX];
    OrbisWatchPoint* Watchpoints[WATCHPOINTS_MAX];

    char CurrentProcName[0x20] = { };
    bool IsDebuggerAttached = false;
    int DebuggerHostIPAddr = 0;
    bool IsRunning = false;

public:
    static void OnTrapFatalHook(trapframe_s *frame);
    static void WatcherThread(void* arg);

    OrbisDebugger(OrbisProc* orbisProc);
    ~OrbisDebugger();

    bool Dbg_CallSetup(int Socket, proc** proc);

    void Dbg_Start(int Socket, int IPAddress);
    void Dbg_Stop(int Socket);
    void Dbg_Break(int Socket);
    void Dbg_Resume(int Socket);
    void Dbg_Step(int Socket);
    void Dbg_StepOver(int Socket);
    void Dbg_StepOut(int Socket);
    void Dbg_GetCallStack(int Socket);

    void APIHandle(int Socket, API_Packet_s* Packet);
};
