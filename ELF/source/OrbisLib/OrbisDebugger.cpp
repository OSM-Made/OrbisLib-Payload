#include "../Main.hpp"
#include "OrbisDebugger.hpp"

void OrbisDebugger::OnTrapFatalHook(trapframe_s *frame)
{
	DebugLog(LOGTYPE_INFO, "Trap Fatal Hit!");

    for(;;){}
}

void OrbisDebugger::WatcherThread(void* arg)
{
    OrbisLib* orbisLib = (OrbisLib*)arg;
    OrbisProc* orbisProc = orbisLib->orbisProc;
    OrbisDebugger* orbisDebugger = orbisLib->orbisDebugger;

    size_t n = 0;
	uint8_t int3 = 0xCC;
	reg Registers;
    proc* proc = 0;
    thread* td = 0;
    static uint64_t PreviousBreakAddress = 0;

    while(orbisDebugger->IsRunning)
    {
        kthread_suspend_check();

        pause("", 10);

        if(!orbisProc->CurrentlyAttached)
            continue;

        proc = proc_find_by_name(orbisProc->CurrentProcName);
        if(!proc)
            continue;

        td = curthread();
        if(!td)
            continue;
        
        int status = 0;
        int res = kwait4(proc->p_pid, &status, WUNTRACED, 0);
        int Signal = WSTOPSIG(status);

        DebugLog(LOGTYPE_INFO, "Res = %d, Status = %d, Signal = %d\n", res, status, WSTOPSIG(status));

        switch (Signal)
        {
        case SIGTRAP:
            
            break;
        }
    }

    kproc_exit(0); 
}

OrbisDebugger::OrbisDebugger(OrbisProc* orbisProc)
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");

    this->orbisProc = orbisProc;
    this->IsRunning = true;

    //Initialize Breakpoints
    for(int i = 0; i < BREAKPOINTS_MAX; i++)
        this->Breakpoints[i] = new OrbisBreakPoint();

    //Initialize Watchpoints
    for(int i = 0; i < WATCHPOINTS_MAX; i++)
        this->Watchpoints[i] = new OrbisWatchPoint();

    //Detour* OnTrapFatalDetour = new Detour((void*)resolve(addr_trap_fatalHook), (void*)OnTrapFatalHook, 17);
}

OrbisDebugger::~OrbisDebugger()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");

    this->IsRunning = false;

    //Free the Breakpoint Classes
    for(int i = 0; i < BREAKPOINTS_MAX; i++)
        delete this->Breakpoints[i];

    //Free the Watchpoint Classes
    for(int i = 0; i < WATCHPOINTS_MAX; i++)
        delete this->Watchpoints[i];

    //Remove Hook??

}

void OrbisDebugger::Dbg_Start(int Socket, int IPAddress)
{
    //Set up data values for debugging and now send traps to debugger.
    IsDebuggerAttached = true;
    DebuggerHostIPAddr = IPAddress;

    //Send the sucess status
    SendStatus(Socket, API_OK);
}

void OrbisDebugger::Dbg_Stop(int Socket)
{
    //Reset data values for next connection.
    IsDebuggerAttached = false;
    DebuggerHostIPAddr = 0;

    //Send the sucess status
    SendStatus(Socket, API_OK);
}

bool OrbisDebugger::Dbg_CallSetup(int Socket, proc** proc)
{
    if(!IsDebuggerAttached)
    {
        DebugLog(LOGTYPE_ERR, "Debugger not attached.");

        SendStatus(Socket, API_ERROR_DEBUGGER_NOT_ATTACHED);

        return false;
    }

    return orbisProc->API_CallSetup(Socket, proc);
}

void OrbisDebugger::Dbg_Break(int Socket)
{
    proc* proc = 0;
    int err = 0;
    thread* td = curthread();

    //Get our Process and make sure were attached.
    if(Dbg_CallSetup(Socket, &proc))
        return;

    err = kpsignal(proc, SIGSTOP);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "psignal SIGSTOP failed %d.", err);

        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }
}

void OrbisDebugger::Dbg_Resume(int Socket)
{
    proc* proc = 0;
    int err = 0;
    thread* td = curthread();

    //Get our Process and make sure were attached.
    if(Dbg_CallSetup(Socket, &proc))
        return;
    
    //Call ptrace PT_CONTINUE to resume exec
    err = kptrace(td, PT_CONTINUE, proc->p_pid, (void*)1, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_CONTINUE failed %d.", err);

        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    //Send status OK
    SendStatus(Socket, API_OK);
}

void OrbisDebugger::Dbg_Step(int Socket)
{
    proc* proc = 0;
    int err = 0;
    thread* td = curthread();

    //Make sure the process execution is stopped.
    if(!(proc->p_flag & 0x20000))
    {
        DebugLog(LOGTYPE_ERR, "Process is Executing. We cant do this unless the execution is stopped.");

        SendStatus(Socket, API_ERROR_PROC_RUNNING);

        return;
    }

    //Get our Process and make sure were attached.
    if(Dbg_CallSetup(Socket, &proc))
        return;

    err = kptrace(td, PT_STEP, proc->p_pid, (void*)1, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_STEP failed %d.", err);

        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    SendStatus(Socket, API_OK);
}

void OrbisDebugger::Dbg_StepOver(int Socket) //StepInto
{

}

void OrbisDebugger::Dbg_StepOut(int Socket)
{

}

void OrbisDebugger::Dbg_GetCallStack(int Socket)
{
    proc* proc = 0;
    int err = 0;
    thread* td = curthread();
    reg Registers;

    //Make sure the process execution is stopped.
    if(!(proc->p_flag & 0x20000))
    {
        DebugLog(LOGTYPE_ERR, "Process is Executing. We cant do this unless the execution is stopped.");

        SendStatus(Socket, API_ERROR_PROC_RUNNING);

        return;
    }

    //Get our Process and make sure were attached.
    if(Dbg_CallSetup(Socket, &proc))
        return;
    

}

void OrbisDebugger::APIHandle(int Socket, API_Packet_s* Packet)
{
	switch(Packet->cmd)
	{
        case API_DBG_START:
            Dbg_Start(Socket, orbisLib->HostIPAddr);
            break;
        
        case API_DBG_STOP:
            Dbg_Stop(Socket);
            break;

        case API_DBG_BREAK:
            Dbg_Break(Socket);
            break;

        case API_DBG_RESUME:
            Dbg_Resume(Socket);
            break;
        
        case API_DBG_STEP:
            Dbg_Step(Socket);
            break;
        
        case API_DBG_STEP_OVER:
            Dbg_StepOver(Socket);
            break;
        
        case API_DBG_STEP_OUT:
            Dbg_StepOut(Socket);
            break;
                    
        case API_DBG_GET_REG:

            break;
                    
        case API_DBG_SET_REG:

            break;
                    
        case API_DBG_GET_FREG:

            break;
                    
        case API_DBG_SET_FREG:

            break;
                    
        case API_DBG_GET_DBGREG:

            break;
                    
        case API_DBG_SET_DBGREG:

            break;

        case API_DBG_GET_CALLSTACK:

            break;
    }   
}