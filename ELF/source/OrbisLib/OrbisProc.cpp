#include "../main.hpp"
#include "OrbisProc.hpp"

/*void OnTrapFatalHook(trapframe *frame)
{
	DebugLog(LOGTYPE_INFO, "Trap Fatal Hit!");
}*/

OrbisProc::OrbisProc()
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");

    orbisShellCode = new OrbisShellCode();

    //Detour* OnTrapFatalDetour = new Detour((void*)resolve(addr_trap_fatalHook), (void*)OnTrapFatalHook, 17);
    ProcessExitEvent = EVENTHANDLER_REGISTER(process_exit,(void*)OnProcessExit, this, EVENTHANDLER_PRI_ANY);

    IsRunning = true;
}

OrbisProc::~OrbisProc()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");

    _free(orbisShellCode);

    IsRunning = false;
}

void OrbisProc::OnProcessExit(void *arg, struct proc *p)
{
    OrbisProc* orbisProc = (OrbisProc*)arg;

    if(strcmp(p->p_comm, orbisProc->CurrentProcName))
        return;
    
    char ProcName[0x20];
    proc* proc = NULL;
    thread* td = curthread();
    int err = 0;

    if(!orbisProc->CurrentlyAttached)
    {
        DebugLog(LOGTYPE_INFO, "Not currently attached to any process.");
        return;
    }

    strcpy(ProcName, orbisProc->CurrentProcName);
    proc = proc_find_by_name(ProcName);
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\" it might have been killed.", ProcName);

        //Reset Data Values
        orbisProc->CurrentProcessID = -1;
        memset(&orbisProc->CurrentProcName[0], 0, sizeof(CurrentProcName));
        orbisProc->CurrentlyAttached = false;

        return;
    }

    DebugLog(LOGTYPE_INFO, "Detaching from process \"%s\".", ProcName);

    //Clear any breakpoints or watchpoints set.
    //TODO: Implement

    //clear shell code from last process.
    orbisProc->orbisShellCode->DestroyShellCode();

    err = kptrace(td, PT_DETACH, orbisProc->CurrentProcessID, (void*)SIGCONT, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_DETACH failed %d.", err);

        return;
    }

     //Reset Data Values
    orbisProc->CurrentProcessID = -1;
    memset(&orbisProc->CurrentProcName[0], 0, sizeof(CurrentProcName));
    orbisProc->CurrentlyAttached = false;

    DebugLog(LOGTYPE_INFO, "Detached from process \"%s\".", ProcName);
}

void OrbisProc::WatcherThread(void* arg)
{
    OrbisProc* orbisProc = (OrbisProc*)arg;
    size_t n = 0;
	uint8_t int3 = 0xCC;
	reg Registers;
    proc* proc = 0;
    thread* td = 0;
    static uint64_t PreviousBreakAddress = 0;

    while(orbisProc->IsRunning)
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

        if(Signal == SIGTRAP)
        {
            //Clearout Register Structure.
            memset(&Registers, 0, sizeof(struct reg));

            //Get registers
            if(kptrace(td, PT_GETREGS, proc->p_pid, (void*)&Registers, 0))
                continue;

            if(Registers.r_rip - 1 == 0xC5CAF0)
            {
                Log("Software Breakpoint Hit - 0x%llX (0x%X)", Registers.r_rip - 1, 0x55);

                uint8_t tempop = 0x55;
                uint64_t Address = 0xC5CAF0;

                //restore original OP code
                n = 0;
                if(proc_rw_mem(proc, (void*)Address, (size_t)0x01, (void*)&tempop, &n, 1))
                    continue;
                
                //Back step one byte
                Registers.r_rip -= 1;
                if(kptrace(td, PT_SETREGS, proc->p_pid, (void*)&Registers, 0))
                    continue;

                //Step to execute break instruction
                if(kptrace(td, PT_STEP, proc->p_pid, (void *)1, 0))
                    continue;

                int status = 0;
                int res = kwait4(proc->p_pid, &status, WUNTRACED, 0);
                int Signal = WSTOPSIG(status);

                DebugLog(LOGTYPE_INFO, "Res = %d, Status = %d, Signal = %d\n", res, status, WSTOPSIG(status));

                if(kptrace(td, PT_CONTINUE, proc->p_pid, (void*)1, 0))
                    continue;

                Log("Software Breakpoint Handled!");
            }
        }
    }

    kproc_exit(0); 
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

    //Allocate space on the heap to send our read data to the host machine.
    Buffer = (char*)_malloc(len);
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
    _free(Buffer);
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

    //Allocate heap space to temporarily sore our data to be written.
    Buffer = (char*)_malloc(len);
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
    _free(Buffer);
}
  
void OrbisProc::Proc_Kill(int Socket, char* ProcName)
{
    proc* proc = 0;
    thread* td = 0;
    int err = 0;
    char* Buffer = 0;
    char KillProcName[0x20] = { 0 };
    bool IsAttachedProc = false;

    if(!strcmp(ProcName, ""))
    {
        strcpy(KillProcName, CurrentProcName);

        //Make sure were are attached to a process.
        if(!CurrentlyAttached)
        {
            DebugLog(LOGTYPE_INFO, "Not currently attached to any process.");

            SendStatus(Socket, false);
            return;
        }
    }
    else
        strcpy(KillProcName, ProcName);

    proc = proc_find_by_name(KillProcName);
    td = curthread();
    
    //Make sure the process still exists.
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", KillProcName);

        if(!IsAttachedProc)
            return;

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

    /*if(!IsAttachedProc)
    {
        //Clear any breakpoints or watchpoints set.
        //TODO: Implement

        //clear shell code from last process.
        orbisShellCode->DestroyShellCode(); //Probably dont need to do this since were not gracefully shutting down the process

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
    }*/

    SendStatus(Socket, true);
}

void OrbisProc::Proc_LoadSPRX(int Socket, const char *name, unsigned int flags)
{
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int Handle = 0;
    vnode* fd_rdir = 0, *fd_jdir = 0;

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

    DebugLog(LOGTYPE_INFO, "Loading SPRX \"%s\"", name);

    SendStatus(Socket, true);

    filedesc* fd = proc->p_fd;
    fd_rdir = fd->fd_rdir;
    fd_jdir = fd->fd_jdir;

    fd->fd_rdir = *(vnode**)resolve(addr_rootvnode); //rootvnode
    fd->fd_jdir = *(vnode**)resolve(addr_rootvnode); //rootvnode

    Handle = this->orbisShellCode->sceKernelLoadStartModule(name, 0, 0, flags, 0, 0);

    fd->fd_rdir = fd_rdir;
    fd->fd_jdir = fd_jdir;

    Send(Socket, (char*)&Handle, sizeof(int));
}

void OrbisProc::Proc_UnloadSPRX(int Socket, int handle, uint32_t flags)
{
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int Result = 0;

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

    SendStatus(Socket, true);

    Result = this->orbisShellCode->sceKernelStopUnloadModule(handle, 0, 0, flags, 0, 0);

    Send(Socket, (char*)&Result, sizeof(int));
}

void OrbisProc::Proc_ReloadSPRX(int Socket, const char *name)
{
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int Handle = -1;
    int Result = 0;
    char Path[0x100] = { 0 };
    vnode* fd_rdir = 0, *fd_jdir = 0;

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

    //Find the module handle and path from the name.
    dynlib* m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        if(!strcmp(basename(m_library->ModulePath), name))
        {
			Handle = m_library->ModuleHandle;
            strcpy(Path, (char*)m_library->ModulePath);
            break;
        }

        m_library = m_library->dynlib_next;
    }

    //Make sure we found the module handle.
    if(Handle == -1)
    {
        SendStatus(Socket, false);
        return;
    }

    Result = this->orbisShellCode->sceKernelStopUnloadModule(Handle, 0, 0, 0, 0, 0);

    //Make Sure we unloaded the module.
    if(Result)
    {
        SendStatus(Socket, false);
        return;
    }

    //Sleep for a short time so out module can unload.
    pause("", 200);

    filedesc* fd = proc->p_fd;
    fd_rdir = fd->fd_rdir;
    fd_jdir = fd->fd_jdir;

    fd->fd_rdir = *(vnode**)resolve(addr_rootvnode); //rootvnode
    fd->fd_jdir = *(vnode**)resolve(addr_rootvnode); //rootvnode

    Handle = this->orbisShellCode->sceKernelLoadStartModule(Path, 0, 0, 0, 0, 0);

    fd->fd_rdir = fd_rdir;
    fd->fd_jdir = fd_jdir;

    //Make sure we loaded the module.
    if(Handle == 0)
    {
        SendStatus(Socket, false);
        return;
    }

    //Send the success status and the module handle
    SendStatus(Socket, true);

    Send(Socket, (char*)&Handle, sizeof(int));
}

void OrbisProc::Proc_ReloadSPRX(int Socket, int Handle)
{
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int Result = 0;
    char Path[0x100] = { 0 };
    vnode* fd_rdir = 0, *fd_jdir = 0;

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

    //Find the module handle and path from the name.
    dynlib* m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        if(m_library->ModuleHandle == Handle)
        {
            strcpy(Path, (char*)m_library->ModulePath);
            break;
        }

        m_library = m_library->dynlib_next;
    }

    //Make sure we found the module Path.
    if(!strcmp(Path, ""))
    {
        SendStatus(Socket, false);
        return;
    }

    Result = this->orbisShellCode->sceKernelStopUnloadModule(Handle, 0, 0, 0, 0, 0);

    //Make Sure we unloaded the module.
    if(Result)
    {
        SendStatus(Socket, false);
        return;
    }

    //Sleep for a short time so out module can unload.
    pause("", 200);

    filedesc* fd = proc->p_fd;
    fd_rdir = fd->fd_rdir;
    fd_jdir = fd->fd_jdir;

    fd->fd_rdir = *(vnode**)resolve(addr_rootvnode); //rootvnode
    fd->fd_jdir = *(vnode**)resolve(addr_rootvnode); //rootvnode

    Handle = this->orbisShellCode->sceKernelLoadStartModule(Path, 0, 0, 0, 0, 0);

    fd->fd_rdir = fd_rdir;
    fd->fd_jdir = fd_jdir;

    //Make sure we loaded the module.
    if(Handle == 0)
    {
        SendStatus(Socket, false);
        return;
    }

    //Send the success status and the module handle
    SendStatus(Socket, true);

    Send(Socket, (char*)&Handle, sizeof(int));
}

void OrbisProc::Proc_GetModuleList(int Socket)
{
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int ModuleCount = 0, SetCount = 0;
    RESP_ModuleList* ModuleList = 0;
    dynlib* m_library = 0;

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

    //Get the number of modules loaded in our attached process.
    m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        ModuleCount ++;
        m_library = m_library->dynlib_next;
    }

    //Allocate memory to store our data.
    ModuleList = (RESP_ModuleList*)_malloc(sizeof(RESP_ModuleList) * ModuleCount);

    //Populate our buffer with data.
    m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        strcpy(ModuleList[SetCount].mName, (char*)basename(m_library->ModulePath));
        strcpy(ModuleList[SetCount].mPath, (char*)m_library->ModulePath);
        ModuleList[SetCount].mHandle = m_library->ModuleHandle;
        ModuleList[SetCount].mTextSegmentBase = (uint64_t)m_library->codeBase;
        ModuleList[SetCount].mTextSegmentLen = m_library->codeSize;
        ModuleList[SetCount].mDataSegmentBase = (uint64_t)m_library->dataBase;
        ModuleList[SetCount].mDataSegmentLen = m_library->dataSize;

        SetCount ++;
        m_library = m_library->dynlib_next;
    }

    SendStatus(Socket, true);

    Send(Socket, (char*)&ModuleCount, sizeof(int));

    Send(Socket, (char*)&ModuleList[0], sizeof(RESP_ModuleList) * ModuleCount);
}