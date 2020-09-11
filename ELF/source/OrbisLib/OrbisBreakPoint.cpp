#include "../Main.hpp"
#include "OrbisBreakPoint.hpp"

void OrbisBreakPoint::OnProcessExit(void *arg, struct proc *p)
{
    OrbisBreakPoint* orbisBreakPoint = (OrbisBreakPoint*)arg;

    if(!strcmp(p->p_comm, orbisBreakPoint->CurrentProcName) && orbisBreakPoint->Used)
    {
        uint64_t OldAddress = orbisBreakPoint->Address;
        orbisBreakPoint->Used = false;
        orbisBreakPoint->Address = 0;
        orbisBreakPoint->RestoreByte = 0x00;
        memset(orbisBreakPoint->CurrentProcName, 0, sizeof(orbisBreakPoint->CurrentProcName));

        DebugLog(LOGTYPE_INFO, "Removed Breakpoint at %llX", OldAddress);
    }
}

void OrbisBreakPoint::Enable(bool State)
{
    if(!Used)
    {
        DebugLog(LOGTYPE_ERR, "Breakpoint not currently being used.");
        return;
    }

    proc* proc = proc_find_by_name(CurrentProcName);
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Couldnt find proc \"%s\"", CurrentProcName);
        return;
    }

    uint8_t NewState = ((Enabled = State)) ? 0xCC : RestoreByte;
    size_t n = 0;
    if(proc_rw_mem(proc, (void*)Address, sizeof(NewState), (void *)&NewState, &n, 1))
    {
        DebugLog(LOGTYPE_ERR, "Couldnt Write Breakpoint(%llX).", Address);
    }

    DebugLog(LOGTYPE_INFO, "Breakpoint(%llX) Has been %s.", Address, (Enabled ? "Enabled" : "Disabled"));
}

void OrbisBreakPoint::Set(char* ProcName, uint64_t Address, bool Enable)
{
    proc* proc = proc_find_by_name(ProcName);
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Couldnt find proc \"%s\"", ProcName);
        return;
    }

    strcpy(CurrentProcName, ProcName);
    this->Address = Address;
    this->HitCount = 0;

    size_t n = 0;
    if(proc_rw_mem(proc, (void*)Address, sizeof(RestoreByte), (void *)&RestoreByte, &n, 0))
    {
        DebugLog(LOGTYPE_ERR, "Couldnt Read Breakpoint(%llX).", Address);
    }

    this->Enable(Enable);

    DebugLog(LOGTYPE_INFO, "Breakpoint set for %llX", Address);
}

void OrbisBreakPoint::Remove()
{
    if(!Used)
    {
        DebugLog(LOGTYPE_ERR, "Breakpoint not currently being used.");
        return;
    }

    proc* proc = proc_find_by_name(CurrentProcName);
    if(proc)
        this->Enable(false);

    uint64_t OldAddress = Address;
    Used = false;
    Address = 0;
    HitCount = 0;
    RestoreByte = 0x00;
    memset(CurrentProcName, 0, sizeof(CurrentProcName));

    DebugLog(LOGTYPE_INFO, "Removed Breakpoint at %llX", OldAddress);
}

void OrbisBreakPoint::HandleHitEvent(reg Registers)
{
    if(!Used)
    {
        DebugLog(LOGTYPE_ERR, "Breakpoint not currently being used.");
        return;
    }

    proc* proc = proc_find_by_name(CurrentProcName);
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Couldnt find proc \"%s\"", CurrentProcName);
        return;
    }

    size_t n = 0;
    uint8_t int3 = 0xCC;
    thread* td = curthread();

    //Restore original byte over writeen by the int3
    if(proc_rw_mem(proc, (void*)Address, (size_t)0x01, (void*)&RestoreByte, &n, 1))
    {
        DebugLog(LOGTYPE_ERR, "Failed to write original byte for Breakpoint(%llX).", Address);
        return;
    }

    //Back step one byte
    Registers.r_rip -= 1;
    if(kptrace(td, PT_SETREGS, proc->p_pid, (void*)&Registers, 0)) 
    {
        DebugLog(LOGTYPE_ERR, "Could not write registers to step back for Breakpoint(%llX).", Address);
        return;
    }

    //Step to execute break instruction
    if(kptrace(td, PT_STEP, proc->p_pid, (void *)1, 0))
    {
        DebugLog(LOGTYPE_ERR, "Could not step forward to execute break instruction for Breakpoint(%llX).", Address);
        return;
    }

    //Wait for Single Step.
    int status = 0;
    int res = kwait4(proc->p_pid, &status, WUNTRACED, 0);

    //Restore int3 for next call.
    if(proc_rw_mem(proc, (void*)Address, (size_t)0x01, (void*)&int3, &n, 1))
    {
        DebugLog(LOGTYPE_ERR, "Failed to write int3 for Breakpoint(%llX).", Address);
        return;
    }

    HitCount++;

    DebugLog(LOGTYPE_INFO, "Software Breakpoint(%llX) Handled!", Address);
}

OrbisBreakPoint::OrbisBreakPoint()
{
    ProcessExitEvent = EVENTHANDLER_REGISTER(process_exit, (void*)OnProcessExit, this, EVENTHANDLER_PRI_ANY);
}

OrbisBreakPoint::~OrbisBreakPoint()
{
    EVENTHANDLER_DEREGISTER(process_exit, ProcessExitEvent);
}