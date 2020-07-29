#include "../main.hpp"
#include "OrbisProc.hpp"

OrbisProc::OrbisProc()
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");

    orbisShellCode = new OrbisShellCode();
}

OrbisProc::~OrbisProc()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");

    _free(orbisShellCode);
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

        //DebugLog(LOGTYPE_INFO, "#%d-%s-%s-%s", ProcList[proc_count].ProcessID, ProcList[proc_count].Attached ? "True" : "False", ProcList[proc_count].ProcName, ProcList[proc_count].TitleID);

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

        SendStatus(Socket, true);
        return;
    }

    //Check to see if we are cureently attached to another process
    if(CurrentlyAttached)
    {
        //Clear any breakpoints or watchpoints set.
        //TODO: Implement

        //clear shell code from last process.
        orbisShellCode->DestroyShellCode();

        //Detach from last process.
        err = kptrace(td, PT_DETACH, CurrentProcessID, (void*)SIGCONT, 0);
        if(err)
        {
            DebugLog(LOGTYPE_ERR, "ptrace PT_DETACH failed %d.", err);
        }

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
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

    pause("Client Thread", 50);

    //Inject shellcode to help with our proc modifications
    orbisShellCode->InstallShellCode(ProcName);

    //Set Current proc attached to.
    CurrentlyAttached = true;
    CurrentProcessID = proc->p_pid;
    strcpy(CurrentProcName, ProcName);

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

    strcpy(ProcName, CurrentProcName);
    proc = proc_find_by_name(ProcName);
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\" it might have been killed.", ProcName);

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
        CurrentlyAttached = false;

        SendStatus(Socket, false);
        return;
    }

    DebugLog(LOGTYPE_INFO, "Detaching from process \"%s\".", ProcName);

    //Clear any breakpoints or watchpoints set.
    //TODO: Implement

    //clear shell code from last process.
    orbisShellCode->DestroyShellCode();

    err = kptrace(td, PT_DETACH, CurrentProcessID, (void*)SIGCONT, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_DETACH failed %d.", err);

        SendStatus(Socket, false);
        return;
    }

     //Reset Data Values
    CurrentProcessID = -1;
    memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
    CurrentlyAttached = false;

    DebugLog(LOGTYPE_INFO, "Detached from process \"%s\".", ProcName);

    SendStatus(Socket, true);
}

void OrbisProc::Proc_GetCurrent(int Socket)
{
    RESP_CurrentProc CurrentProc;
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int err = 0;

    //Make sure were are attached to a process.
    if(!CurrentlyAttached)
    {
        DebugLog(LOGTYPE_INFO, "Not currently attached to any process.");

        SendStatus(Socket, false);
        return;
    }

    //Make sure the process were attached to still exists.
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", CurrentProcName);

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
        CurrentlyAttached = false;

        SendStatus(Socket, false);
        return;
    }

    //Populate the response packet.
    CurrentProc.ProcessID = CurrentProcessID;
    strcpy(CurrentProc.ProcName, CurrentProcName);
    strcpy(CurrentProc.TitleID, proc->titleId);

    //Signal we are attached and we have the data.
    SendStatus(Socket, true);

    //Send the response Packet
    Send(Socket, (char*)&CurrentProc, sizeof(RESP_CurrentProc));
}
int hModule = 0;
void OrbisProc::Proc_Read(int Socket, uint64_t Address, size_t len)
{
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int err = 0;
    char* Buffer = 0;
    
    //Make sure were are attached to a process.
    if(!CurrentlyAttached)
    {
        DebugLog(LOGTYPE_INFO, "Not currently attached to any process.");

        SendStatus(Socket, false);
        return;
    }
    
    //Make sure the process were attached to still exists.
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", CurrentProcName);

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
        CurrentlyAttached = false;

        SendStatus(Socket, false);
        return;
    }

    filedesc* fd = proc->p_fd;
    vnode* old_fd_rdir = fd->fd_rdir;
    vnode* old_fd_jdir = fd->fd_jdir;
    fd->fd_rdir = *(vnode**)resolve(addr_rootvnode); //rootvnode
    fd->fd_jdir = *(vnode**)resolve(addr_rootvnode); //rootvnode

    hModule = orbisShellCode->sceKernelLoadStartModule("/mnt/usb0/Test.sprx", 0, 0, 0, 0, 0);
    DebugLog(LOGTYPE_INFO, "SPRX Handle = %d", hModule);

    fd->fd_rdir = old_fd_rdir;
    fd->fd_jdir = old_fd_jdir;

    dynlib* m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        printf("[%d]%s\n", m_library->ModuleHandle, basename(m_library->LibraryPath));

        m_library = m_library->dynlib_next; 
    }

    SendStatus(Socket, true);

    //Allocate space on the heap to send our read data to the host machine.
    /*Buffer = (char*)_malloc(len);
    if(!Buffer)
    {
        DebugLog(LOGTYPE_ERR, "malloc failed to allocate %d bytes.\n", len);

        SendStatus(Socket, false);
        return;
    }

    memset(Buffer, 0, len);

    size_t n = 0;
    err = proc_rw_mem(proc, (void*)Address, len, Buffer, &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "proc_rw_mem couldnt read memory at Address(0x%llX) Size(%d) Error:%d n:%d.\n", Address, len, err, n);

        SendStatus(Socket, false);

        _free(Buffer);
        return;
    }
    
    //Send a Success Response.
    SendStatus(Socket, true);
    
    //Send Data Read.
    Send(Socket, Buffer, len);
    
    //Clean up.
    _free(Buffer);*/
}

void OrbisProc::Proc_Write(int Socket, uint64_t Address, size_t len)
{
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int err = 0;
    char* Buffer = 0;
    
    //Make sure were are attached to a process.
    if(!CurrentlyAttached)
    {
        DebugLog(LOGTYPE_INFO, "Not currently attached to any process.");

        SendStatus(Socket, false);
        return;
    }
    
    //Make sure the process were attached to still exists.
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", CurrentProcName);

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
        CurrentlyAttached = false;

        SendStatus(Socket, false);
        return;
    }

    int res = orbisShellCode->sceKernelStopUnloadModule(hModule, 0, 0, 0, 0, 0);
    DebugLog(LOGTYPE_INFO, "res = %d", res);

    dynlib* m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        printf("[%d]%s\n", m_library->ModuleHandle, basename(m_library->LibraryPath));

        m_library = m_library->dynlib_next; 
    }

    SendStatus(Socket, true);

    //Allocate heap space to temporarily sore our data to be written.
    /*Buffer = (char*)_malloc(len);
    if(!Buffer)
    {
        DebugLog(LOGTYPE_ERR, "malloc failed to allocate %d bytes.\n", len);

        SendStatus(Socket, false);
        return;
    }

    memset(Buffer, 0, len);

    //Send were ready to recive data.
    SendStatus(Socket, true);

    if(!Receive(Socket, Buffer, len))
    {
        DebugLog(LOGTYPE_ERR, "Receive failed to retrieve the memory to write.\n");

        SendStatus(Socket, false);
    }
    
    //Call proc_rw_mem with the write param as 1 to write our data.
    size_t n = 0;
    err = proc_rw_mem(proc, (void*)Address, len, Buffer, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "proc_read_mem couldnt write memory at Address(0x%llX) Size(%d) Error:%d n:%d.\n", Address, len, err, n);

        SendStatus(Socket, false);

        _free(Buffer);
        return;
    }

    //Send Successfully written data.
    SendStatus(Socket, true);

    //Clean up.
    _free(Buffer);*/
}
  
void OrbisProc::Proc_Kill(int Socket)
{
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int err = 0;
    char* Buffer = 0;
    
    //Make sure were are attached to a process.
    if(!CurrentlyAttached)
    {
        DebugLog(LOGTYPE_INFO, "Not currently attached to any process.");

        SendStatus(Socket, false);
        return;
    }
    
    //Make sure the process were attached to still exists.
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", CurrentProcName);

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
        CurrentlyAttached = false;

        SendStatus(Socket, false);
        return;
    }

    kpsignal(proc, SIGSTOP);
    pause("Client Thread", 150);
    kpsignal(proc, SIGKILL);
    pause("Client Thread", 150);

    //Clear any breakpoints or watchpoints set.
    //TODO: Implement

    //clear shell code from last process.
    //orbisShellCode->DestroyShellCode(); //Probably dont need to do this since were not gracefully shutting down the process

    //Detach from last process.
    err = kptrace(td, PT_DETACH, CurrentProcessID, (void*)SIGCONT, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_DETACH failed %d.", err);
    }

    //Reset Data Values
    CurrentProcessID = -1;
    memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
    CurrentlyAttached = false;

    SendStatus(Socket, true);
}