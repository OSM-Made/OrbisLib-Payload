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

void SendStatus(int Socket, int Status)
{
    Send(Socket, (char*)&Status, 0x4);
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

        SendStatus(Socket, false);
        return;
    }

    //Check to see if we are attached to this process already
    if(CurrentlyAttached && CurrentProcessID == proc->p_pid)
    {
        DebugLog(LOGTYPE_INFO, "Already attached to process %s.", ProcName);

        SendStatus(Socket, false);
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

            SendStatus(Socket, false);
            return;
        }

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProc[0], 0, sizeof(CurrentProc));
        CurrentlyAttached = false;
    }

    DebugLog(LOGTYPE_INFO, "Attaching to process \"%s\".", ProcName);

    //Connect to our new process.
    err = kptrace(td, PT_ATTACH, proc->p_pid, 0, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_ATTACH failed %d.", err);

        SendStatus(Socket, false);
        return;
    }

    pause("Client Thread", 150);

    //Continue execution after attaching to process.
    err = kptrace(td, PT_CONTINUE, proc->p_pid, (void*)1, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_CONTINUE failed %d.", err);

        SendStatus(Socket, false);
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

    DebugLog(LOGTYPE_INFO, "Attached to process \"%s\".", ProcName);

    SendStatus(Socket, true);
}

void OrbisProc::Proc_Detach(int Socket)
{
    char ProcName[0x20];
    proc* proc = NULL;
    thread* td = curthread();
    int err = 0;

    if(!CurrentlyAttached)
    {
        DebugLog(LOGTYPE_INFO, "Not currently attached to any process.");

        SendStatus(Socket, false);
        return;
    }

    strcpy(ProcName, CurrentProc);
    proc = proc_find_by_name(ProcName);
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\" it might have been killed.", ProcName);

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProc[0], 0, sizeof(CurrentProc));
        CurrentlyAttached = false;

        SendStatus(Socket, false);
        return;
    }

    DebugLog(LOGTYPE_INFO, "Detaching from process \"%s\".", ProcName);

    //Clear any breakpoints or watchpoints set.
    //TODO: Implement

    //clear shell code from last process.
    //TODO: Implement

    err = kptrace(td, PT_DETACH, CurrentProcessID, (void*)SIGCONT, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_DETACH failed %d.", err);

        SendStatus(Socket, false);
        return;
    }

     //Reset Data Values
    CurrentProcessID = -1;
    memset(&CurrentProc[0], 0, sizeof(CurrentProc));
    CurrentlyAttached = false;

    DebugLog(LOGTYPE_INFO, "Detached from process \"%s\".", ProcName);

    SendStatus(Socket, true);
}

void OrbisProc::Proc_Read(int Socket, uint64_t Address, size_t len)
{

}

void OrbisProc::Proc_Write(int Socket, uint64_t Address, size_t len)
{

}
  