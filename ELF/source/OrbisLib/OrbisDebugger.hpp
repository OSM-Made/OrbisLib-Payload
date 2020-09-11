#pragma once

#include "OrbisBreakPoint.hpp"
#include "OrbisWatchPoint.hpp"

#define BREAKPOINTS_MAX 10
#define WATCHPOINTS_MAX 4

class OrbisDebugger
{
private:
    OrbisProc* orbisProc;
    OrbisBreakPoint* Breakpoints[BREAKPOINTS_MAX];
    OrbisWatchPoint* Watchpoints[WATCHPOINTS_MAX];

public:
    OrbisDebugger(OrbisProc* orbisProc);
    ~OrbisDebugger();


    void APIHandle(int Socket, API_Packet_s* Packet);
};
