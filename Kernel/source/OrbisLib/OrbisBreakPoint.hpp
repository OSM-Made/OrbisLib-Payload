#pragma once

extern "C"
{
    #include "machine/reg.h";
}

class OrbisBreakPoint
{
private:
    eventhandler_entry* ProcessExitEvent;
    static void OnProcessExit(void *arg, struct proc *p);

public:
    bool Used = false;
    bool Enabled = false;
    uint64_t Address = 0;
    uint8_t RestoreByte = 0x0;
    char CurrentProcName[0x20] = { 0 };
    int32_t HitCount = 0;

    void Enable(bool State);
    void Set(char* ProcName, uint64_t Address, bool Enable);
    void Remove();
    void HandleHitEvent(reg Registers);

    OrbisBreakPoint();
    ~OrbisBreakPoint();
};
