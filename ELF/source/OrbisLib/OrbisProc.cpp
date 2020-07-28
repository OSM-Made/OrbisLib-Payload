#include "../main.hpp"
#include "OrbisProc.hpp"
#include "../Util/Proc.hpp"

OrbisProc::OrbisProc()
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");

}

OrbisProc::~OrbisProc()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");

}

void OrbisProc::Proc_GetList(int Socket)
{
    RESP_ProcList ProcList[100];
    int proc_count = 0;
    proc *allproc = *(proc**)resolve(addr_allproc);

    do
    {
        ProcList[proc_count].ProcessID = allproc->p_pid;
        ProcList[proc_count].Attached = ((allproc->p_flag & 0x800) != 0);
        memcpy(&ProcList[proc_count].ProcName, allproc->p_comm, strlen(allproc->p_comm) + 1);
        memcpy(&ProcList[proc_count].TitleID, allproc->titleId, 10);

        DebugLog(LOGTYPE_INFO, "#%d-%s-%s-%s", ProcList[proc_count].ProcessID, ProcList[proc_count].Attached ? "True" : "False", ProcList[proc_count].ProcName, ProcList[proc_count].TitleID);

        proc_count ++;
        allproc = allproc->p_list.le_next;
    } while (allproc != NULL);
    
    Send(Socket, (char*)&(proc_count), sizeof(int));
    Send(Socket, (char*)&ProcList[0], proc_count * sizeof(RESP_ProcList));
}

void OrbisProc::Proc_Attach(int Socket, char* ProcName)
{
    proc* proc = proc_find_by_name(ProcName);
    thread* td = curthread();
    int err = 0;

    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", ProcName);
        return;
    }

    //Check to see if we are attached to this process already
    if(CurrentlyAttached && CurrentProcessID == proc->p_pid)
    {
        DebugLog(LOGTYPE_INFO, "Already attached to process %s.", ProcName);
        return;
    }

    //Check to see if we are cureently attached to another process
    if(CurrentlyAttached)
    {
        //Clear any breakpoints or watchpoints set.
        //TODO: Implement

        //clear shell code from last process.
        //TODO: Implement

        //Detach from last process.
        err = kptrace(td, PT_DETACH, CurrentProcessID, (void*)SIGCONT, 0);
        if(err)
        {
            DebugLog(LOGTYPE_ERR, "ptrace PT_DETACH failed %d.", err);
            return;
        }

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProc[0], 0, sizeof(CurrentProc));
        CurrentlyAttached = false;
    }

    //Connect to our new process.
    err = kptrace(td, PT_ATTACH, CurrentProcessID, 0, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_ATTACH failed %d.", err);
        return;
    }

    //Inject shellcode to help with our proc modifications
    //TODO:Implement this...

    //Set Current proc attached to.
    CurrentlyAttached = true;
    CurrentProcessID = proc->p_pid;
    strcpy(CurrentProc, ProcName);

    //Notify all current Host instances we have attached to a new proc
    //TODO: Implement... 
}

void OrbisProc::Proc_Detach(int Socket)
{
    
}

void OrbisProc::Proc_Read(int Socket, uint64_t Address, size_t len)
{

}

void OrbisProc::Proc_Write(int Socket, uint64_t Address, size_t len)
{

}
  