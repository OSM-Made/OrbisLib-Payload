#include "../main.hpp"
#include "OrbisProc.hpp"
#include "../Util/ELFLoader.hpp"

OrbisProc::OrbisProc()
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");

    //Initialize shellcode Class
    orbisShellCode = new OrbisShellCode();

    //Register Events
    ProcessStartEvent = EVENTHANDLER_REGISTER(process_exec, (void*)OnProcessStart, this, EVENTHANDLER_PRI_ANY);
    ProcessExitEvent = EVENTHANDLER_REGISTER(process_exit, (void*)OnProcessExit, this, EVENTHANDLER_PRI_ANY);

    IsRunning = true;
}

OrbisProc::~OrbisProc()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");

    EVENTHANDLER_DEREGISTER(process_exit, ProcessExitEvent);

    delete orbisShellCode;

    IsRunning = false;
}

void OrbisProc::OnProcessStart(void *arg, struct proc *p)
{
    OrbisProc* orbisProc = (OrbisProc*)arg;

    if(strstr(p->titleId, "CUSA"))
    {
        pTargetComms->SendNewTitle(p->titleId);

        Log("New Title started! (%s)", p->titleId);
    }
}

void OrbisProc::OnProcessExit(void *arg, struct proc *p)
{
    OrbisProc* orbisProc = (OrbisProc*)arg;

    if(strstr(p->titleId, "CUSA"))
    {
        pTargetComms->SendNewTitle("XMB");

        Log("Returning to XMB!");
    }

    Log("Process Exit (%s).", p->p_comm);

    //If were connected to a target detach from the process
    if(strcmp(p->p_comm, orbisProc->CurrentProcName))
        return;

    //Let the host know the process has died.
   pTargetComms->SendTargetCommand(CMD_PROC_DIE);

     //Reset Data Values
    orbisProc->CurrentProcessID = -1;
    memset(&orbisProc->CurrentProcName[0], 0, sizeof(CurrentProcName));
    orbisProc->CurrentlyAttached = false;
}

void OrbisProc::Proc_GetList(int Socket)
{
    RESP_Proc ProcList[100];
    int proc_count = 0;
    proc *allproc = *(proc**)resolve(addr_allproc);

    do
    {
        ProcList[proc_count].ProcessID = allproc->p_pid;
        ProcList[proc_count].Attached = ((allproc->p_flag & 0x800) != 0);
        memcpy(&ProcList[proc_count].ProcName, allproc->p_comm, strlen(allproc->p_comm) + 1);
        memcpy(&ProcList[proc_count].TitleID, allproc->titleId, 10);
        ProcList[proc_count].TextSegmentBase = (uint64_t)allproc->p_vmspace->vm_taddr;
        ProcList[proc_count].TextSegmentLen = (uint64_t)(allproc->p_vmspace->vm_tsize * PAGE_SIZE);
        ProcList[proc_count].DataSegmentBase = (uint64_t)allproc->p_vmspace->vm_daddr;
        ProcList[proc_count].DataSegmentLen = (uint64_t)(allproc->p_vmspace->vm_dsize * PAGE_SIZE);

        //DebugLog(LOGTYPE_INFO, "#%d-%s-%s-%s", ProcList[proc_count].ProcessID, ProcList[proc_count].Attached ? "True" : "False", ProcList[proc_count].ProcName, ProcList[proc_count].TitleID);

        proc_count ++;
        allproc = allproc->p_list.le_next;
    } while (allproc != NULL);

    SendStatus(Socket, API_OK);
    
    Send(Socket, (char*)&proc_count, sizeof(int));
    Send(Socket, (char*)&ProcList[0], proc_count * sizeof(RESP_Proc));
}

int OrbisProc::API_CallSetup(int Socket, proc** proc)
{
    //Make sure were are attached to a process.
    if(!CurrentlyAttached)
    {
        DebugLog(LOGTYPE_INFO, "Not currently attached to any process.");

        SendStatus(Socket, API_ERROR_NOT_ATTACHED);
        return API_ERROR_NOT_ATTACHED;
    }
    
    //Make sure the process were attached to still exists.
    *proc = proc_find_by_name(CurrentProcName);
    if(!*proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", CurrentProcName);

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
        CurrentlyAttached = false;

        SendStatus(Socket, API_ERROR_LOST_PROC);
        return API_ERROR_LOST_PROC;
    }

    return API_OK;
}

void OrbisProc::Proc_Attach(int Socket, char* ProcName)
{
    proc* proc = proc_find_by_name(ProcName);
    thread* td = curthread();
    int err = 0;

    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", ProcName);

        SendStatus(Socket, API_ERROR_LOST_PROC);
        return;
    }

    //Check to see if we are attached to this process already
    if(CurrentlyAttached && CurrentProcessID == proc->p_pid)
    {
        DebugLog(LOGTYPE_INFO, "Already attached to process %s.", ProcName);

        SendStatus(Socket, API_OK);
        
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
    err = kptrace(td, PT_ATTACH, proc->p_pid, (void*)SIGCONT, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_ATTACH failed %d.", err);

        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    pause("Client Thread", 150);

    //Continue execution after attaching to process.
    err = kptrace(td, PT_CONTINUE, proc->p_pid, (void*)1, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_CONTINUE failed %d.", err);

        SendStatus(Socket, API_ERROR_FAIL);
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
    pTargetComms->SendProcChange(ProcName);

    DebugLog(LOGTYPE_INFO, "Attached to process \"%s\".", ProcName);

    SendStatus(Socket, API_OK);
}

void OrbisProc::Proc_Detach(int Socket, char* ProcName)
{
    proc* proc = NULL;
    thread* td = curthread();
    int err = 0;

    if(!strcmp(CurrentProcName, ProcName) && !CurrentlyAttached)
    {
        DebugLog(LOGTYPE_INFO, "Not currently attached to any process.");

        SendStatus(Socket, API_ERROR_NOT_ATTACHED);
        return;
    }

    proc = proc_find_by_name(ProcName);
    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\" it might have been killed.", ProcName);

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
        CurrentlyAttached = false;

        SendStatus(Socket, API_ERROR_LOST_PROC);
        return;
    }

    DebugLog(LOGTYPE_INFO, "Detaching from process \"%s\".", ProcName);

    if(!strcmp(CurrentProcName, ProcName))
    {
        //Clear any breakpoints or watchpoints set.
        //TODO: Implement

        //clear shell code from last process.
        orbisShellCode->DestroyShellCode();
    }

    err = kptrace(td, PT_DETACH, CurrentProcessID, (void*)SIGCONT, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "ptrace PT_DETACH failed %d.", err);

        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    if(!strcmp(CurrentProcName, ProcName))
    {

        //Reset Data Values
        CurrentProcessID = -1;
        memset(&CurrentProcName[0], 0, sizeof(CurrentProcName));
        CurrentlyAttached = false;

        //Notify the host instances that we have detached from the current process.
        pTargetComms->SendTargetCommand(CMD_PROC_DETACH);
    }

    DebugLog(LOGTYPE_INFO, "Detached from process \"%s\".", ProcName);

    SendStatus(Socket, API_OK);
}

void OrbisProc::Proc_GetCurrent(int Socket)
{
    RESP_Proc CurrentProc;
    proc* proc = 0;
    thread* td = curthread();
    int err = 0;

    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

    //Populate the response packet.
    CurrentProc.ProcessID = CurrentProcessID;
    CurrentProc.Attached = ((proc->p_flag & 0x800) != 0);
    strcpy(CurrentProc.ProcName, CurrentProcName);
    strcpy(CurrentProc.TitleID, proc->titleId);
    CurrentProc.TextSegmentBase = (uint64_t)proc->p_vmspace->vm_taddr;
    CurrentProc.TextSegmentLen = (uint64_t)(proc->p_vmspace->vm_tsize * PAGE_SIZE);
    CurrentProc.DataSegmentBase = (uint64_t)proc->p_vmspace->vm_daddr;
    CurrentProc.DataSegmentLen = (uint64_t)(proc->p_vmspace->vm_dsize * PAGE_SIZE);

    //Signal we are attached and we have the data.
    SendStatus(Socket, API_OK);

    //Send the response Packet
    Send(Socket, (char*)&CurrentProc, sizeof(RESP_Proc));
}

void OrbisProc::Proc_Read(int Socket, uint64_t Address, size_t len)
{
    proc* proc = 0;
    thread* td = curthread();
    int err = 0;
    char* Buffer = 0;
    
    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

    //make sure we are peeking the process memory space
    if(Address > (uint64_t)(proc->p_vmspace->vm_taddr + ((proc->p_vmspace->vm_tsize + proc->p_vmspace->vm_dsize) * PAGE_SIZE)) || Address < (uint64_t)proc->p_vmspace->vm_taddr)
    {
        DebugLog(LOGTYPE_ERR, "Address 0x%llX is out side the memory space of %s 0x%llX -> 0x%llX.", Address, CurrentProcName, proc->p_vmspace->vm_taddr, (proc->p_vmspace->vm_taddr + ((proc->p_vmspace->vm_tsize + proc->p_vmspace->vm_dsize) * PAGE_SIZE)));

        SendStatus(Socket, API_ERROR_INVALID_ADDRESS);

        return;
    }

    //Allocate space on the heap to send our read data to the host machine.
    Buffer = (char*)_malloc(len);
    if(!Buffer)
    {
        DebugLog(LOGTYPE_ERR, "malloc failed to allocate %d bytes.\n", len);

        SendStatus(Socket, API_ERROR_FAIL);

        return;
    }

    memset(Buffer, 0, len);

    size_t n = 0;
    err = proc_rw_mem(proc, (void*)Address, len, Buffer, &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "proc_rw_mem couldnt read memory at Address(0x%llX) Size(%d) Error:%d n:%d.\n", Address, len, err, n);

        SendStatus(Socket, API_ERROR_FAIL);

        _free(Buffer);
        return;
    }
    
    //Send a Success Response.
    SendStatus(Socket, API_OK);
    
    //Send Data Read.
    Send(Socket, Buffer, len);
    
    //Clean up.
    _free(Buffer);
}

void OrbisProc::Proc_Write(int Socket, uint64_t Address, size_t len)
{
    proc* proc = 0;
    thread* td = curthread();
    int err = 0;
    char* Buffer = 0;
    
    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

    //make sure we are writing the process memory space
    if(Address > (uint64_t)(proc->p_vmspace->vm_taddr + ((proc->p_vmspace->vm_tsize + proc->p_vmspace->vm_dsize) * PAGE_SIZE)) || Address < (uint64_t)proc->p_vmspace->vm_taddr)
    {
        DebugLog(LOGTYPE_ERR, "Address 0x%llX is out side the memory space of %s 0x%llX -> 0x%llX.", Address, CurrentProcName, proc->p_vmspace->vm_taddr, (proc->p_vmspace->vm_taddr + ((proc->p_vmspace->vm_tsize + proc->p_vmspace->vm_dsize) * PAGE_SIZE)));

        SendStatus(Socket, API_ERROR_INVALID_ADDRESS);

        return;
    }

    //Allocate heap space to temporarily sore our data to be written.
    Buffer = (char*)_malloc(len);
    if(!Buffer)
    {
        DebugLog(LOGTYPE_ERR, "malloc failed to allocate %d bytes.\n", len);

        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    memset(Buffer, 0, len);

    //Send were ready to recive data.
    SendStatus(Socket, API_OK);

    if(!Receive(Socket, Buffer, len))
    {
        DebugLog(LOGTYPE_ERR, "Receive failed to retrieve the memory to write.\n");

        SendStatus(Socket, API_ERROR_FAIL);

        _free(Buffer);

        return;
    }
    
    //Call proc_rw_mem with the write param as 1 to write our data.
    size_t n = 0;
    err = proc_rw_mem(proc, (void*)Address, len, Buffer, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "proc_read_mem couldnt write memory at Address(0x%llX) Size(%d) Error:%d n:%d.\n", Address, len, err, n);

        SendStatus(Socket, API_ERROR_FAIL);

        _free(Buffer);
        return;
    }

    //Send Successfully written data.
    SendStatus(Socket, API_OK);

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

            SendStatus(Socket, API_ERROR_NOT_ATTACHED);
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

        SendStatus(Socket, API_ERROR_LOST_PROC);
        return;
    }

    kpsignal(proc, SIGSTOP);
    pause("Client Thread", 150);
    kpsignal(proc, SIGKILL);
    pause("Client Thread", 150);

    SendStatus(Socket, API_OK);
}

void OrbisProc::Proc_LoadELF(int Socket, size_t Len)
{
    proc* proc = 0;
    thread* td = curthread();
    int err = 0;
    char* Buffer = 0;
    
    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

    //Allocate heap space to temporarily sore our ELF to be written.
    Buffer = (char*)_malloc(Len);
    if(!Buffer)
    {
        DebugLog(LOGTYPE_ERR, "malloc failed to allocate %d bytes.\n", Len);

        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    memset(Buffer, 0, Len);

    //Send were ready to recive data.
    SendStatus(Socket, API_OK);

    if(!Receive(Socket, Buffer, Len))
    {
        DebugLog(LOGTYPE_ERR, "Receive failed to retrieve the memory to write.\n");

        SendStatus(Socket, API_ERROR_FAIL);

        _free(Buffer);

        return;
    }
    
    //Load the elf.
    if(proc_LoadELF(proc, Buffer) != 0) 
    {
        DebugLog(LOGTYPE_ERR, "Failed to load ELF on process \"%s\".", CurrentProcName);

        SendStatus(Socket, API_ERROR_FAIL);
    }
    else
        SendStatus(Socket, API_OK);

    //Clean up.
    _free(Buffer);
}

void OrbisProc::Proc_Call(int Socket)
{
    DebugLog(LOGTYPE_WARN, "Not Implimented!");
    SendStatus(Socket, API_ERROR_FAIL);
}

void OrbisProc::Proc_LoadSPRX(int Socket, const char *name, uint32_t flags)
{
    proc* proc = 0;
    thread* td = curthread();
    int Handle = 0;
    vnode* fd_rdir = 0, *fd_jdir = 0;

    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

    SendStatus(Socket, API_OK);

    //back up old file perms
    filedesc* fd = proc->p_fd;
    fd_rdir = fd->fd_rdir;
    fd_jdir = fd->fd_jdir;

    //Give us root file perms temporarily.
    fd->fd_rdir = *(vnode**)resolve(addr_rootvnode); //rootvnode
    fd->fd_jdir = *(vnode**)resolve(addr_rootvnode); //rootvnode

    Handle = this->orbisShellCode->sceKernelLoadStartModule(name, 0, 0, flags, 0, 0);

    //retstore old file perms.
    fd->fd_rdir = fd_rdir;
    fd->fd_jdir = fd_jdir;

    Send(Socket, (char*)&Handle, sizeof(int));
}

void OrbisProc::Proc_UnloadSPRX(int Socket, int32_t Handle, uint32_t flags)
{
    proc* proc = 0;
    thread* td = curthread();
    int Result = 0;

    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

    SendStatus(Socket, API_OK);

    Result = this->orbisShellCode->sceKernelStopUnloadModule(Handle, 0, 0, flags, 0, 0);

    Send(Socket, (char*)&Result, sizeof(int));
}

void OrbisProc::Proc_UnloadSPRX(int Socket, const char* Name, uint32_t flags)
{
    proc* proc = 0;
    int Handle = -1;
    thread* td = curthread();
    int Result = 0;

    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

    //Find the module handle and path from the name.
    dynlib* m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        if(!strcmp(basename(m_library->ModulePath), Name))
        {
			Handle = m_library->ModuleHandle;
            break;
        }

        m_library = m_library->dynlib_next;
    }

    //Make sure we found the module handle.
    if(Handle == -1)
    {
        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    SendStatus(Socket, API_OK);

    Result = this->orbisShellCode->sceKernelStopUnloadModule(Handle, 0, 0, flags, 0, 0);

    Send(Socket, (char*)&Result, sizeof(int));
}

void OrbisProc::Proc_ReloadSPRX(int Socket, const char *name)
{
    proc* proc = 0;
    thread* td = curthread();
    int Handle = -1;
    int Result = 0;
    char Path[0x100] = { 0 };
    vnode* fd_rdir = 0, *fd_jdir = 0;

    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

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
        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    Result = this->orbisShellCode->sceKernelStopUnloadModule(Handle, 0, 0, 0, 0, 0);

    //Make Sure we unloaded the module.
    if(Result)
    {
        SendStatus(Socket, API_ERROR_FAIL);
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
        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    //Send the success status and the module handle
    SendStatus(Socket, API_OK);

    Send(Socket, (char*)&Handle, sizeof(int));
}

void OrbisProc::Proc_ReloadSPRX(int Socket, int Handle)
{
    proc* proc = 0;
    thread* td = curthread();
    int Result = 0;
    char Path[0x100] = { 0 };
    vnode* fd_rdir = 0, *fd_jdir = 0;

    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

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
        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    Result = this->orbisShellCode->sceKernelStopUnloadModule(Handle, 0, 0, 0, 0, 0);

    //Make Sure we unloaded the module.
    if(Result)
    {
        SendStatus(Socket, API_ERROR_FAIL);
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
        SendStatus(Socket, API_ERROR_FAIL);
        return;
    }

    //Send the success status and the module handle
    SendStatus(Socket, API_OK);

    Send(Socket, (char*)&Handle, sizeof(int));
}

void OrbisProc::Proc_DumpModule(int Socket, const char* ModuleName)
{
    int32_t Size = 0;
	char* DumpedData = 0;
	size_t n = 0;
	int err = 0;
    proc* proc = 0;
    thread* td = curthread();
    uint64_t TextSegBase = 0, TextSegSize = 0, DataSegBase = 0, DataSegSize = 0;
    bool FoundModule = false;

    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

    //Find the module text and data segments.
    dynlib* m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        if(!strcmp(basename(m_library->ModulePath), ModuleName))
        {   
            TextSegBase = m_library->codeBase;
            TextSegSize = m_library->codeSize;
            DataSegBase = m_library->dataBase;
            DataSegSize = m_library->dataSize;

            FoundModule = true;

            break;
        }

        m_library = m_library->dynlib_next;
    }

    //make sure we found the module.
    if(!FoundModule)
    {
        DebugLog(LOGTYPE_ERR, "Failed to Find the module \"%s\" to dump.", ModuleName);

        SendStatus(Socket, API_ERROR_FAIL);

        return;
    }

    //allocate space to dump the module text and data segments.
    Size = (TextSegSize + DataSegSize);
    DumpedData = (char*)_malloc(Size);
	if(!DumpedData)
	{
		DebugLog(LOGTYPE_ERR, "Failed to allocate space for Dump.");
		
		SendStatus(Socket, API_ERROR_FAIL);

		goto Cleanup;
	}

    //Dump the text segment to the start of the buffer.
	err = proc_rw_mem(proc, (void*)TextSegBase, TextSegSize, DumpedData, &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read Text Segment Data: %d(%d).", err, n);

		SendStatus(Socket, API_ERROR_FAIL);

		goto Cleanup;

	}

    //Dump the Data segment to the bottom of the buffer.
	n = 0;
	err = proc_rw_mem(proc, (void*)DataSegBase, DataSegSize, (void*)(DumpedData + TextSegSize), &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read Data Segment Data: %d(%d).", err, n);

		SendStatus(Socket, API_ERROR_FAIL);

		goto Cleanup;
	}

	SendStatus(Socket, API_OK);

    //Send the Size and the dumped data.
	Send(Socket, (char*)&Size, sizeof(int));
	Send(Socket, DumpedData, Size);

    char Buffer[0x200];
	sprintf(Buffer, "Dumped\n%s\n0x%llX", ModuleName, TextSegSize + DataSegSize);
	pHelperManager->pUserlandHelper->sceSysUtilSendNotificationRequest(Buffer);

Cleanup:
	_free(DumpedData);
}

void OrbisProc::Proc_GetModuleList(int Socket)
{
    proc* proc = proc_find_by_name(CurrentProcName);
    thread* td = curthread();
    int ModuleCount = 0, SetCount = 0;
    RESP_ModuleList* ModuleList = 0;
    dynlib* m_library = 0;

    //Get our Process and make sure were attached.
    if(API_CallSetup(Socket, &proc))
        return;

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

    SendStatus(Socket, API_OK);

    Send(Socket, (char*)&ModuleCount, sizeof(int));

    Send(Socket, (char*)&ModuleList[0], sizeof(RESP_ModuleList) * ModuleCount);

    //Cleanup
    _free(ModuleList);
}

void OrbisProc::APIHandle(int Socket, API_Packet_s* Packet)
{
	switch(Packet->cmd)
	{
        case API_PROC_GET_LIST:
            Proc_GetList(Socket);
            break;

        case API_PROC_ATTACH:
            Proc_Attach(Socket, Packet->ProcName);
            break;

        case API_PROC_DETACH:
            Proc_Detach(Socket, Packet->ProcName);
            break;

        case API_PROC_GET_CURRENT:
            Proc_GetCurrent(Socket);
            break;

        case API_PROC_READ:
            Proc_Read(Socket, Packet->Proc_RW.Address, Packet->Proc_RW.len);
            break;

        case API_PROC_WRITE:
            Proc_Write(Socket, Packet->Proc_RW.Address, Packet->Proc_RW.len);
            break;

        case API_PROC_KILL:
            Proc_Kill(Socket, Packet->ProcName);
            break;

        case API_PROC_LOAD_ELF:
            Proc_LoadELF(Socket, Packet->Proc_ELF.Len);
            break;

        case API_PROC_CALL:
            Proc_Call(Socket);
            break;


        /* Remote Library functions */
        case API_PROC_LOAD_SPRX:
            Proc_LoadSPRX(Socket, Packet->Proc_SPRX.ModuleDir, Packet->Proc_SPRX.Flags);
            break;

        case API_PROC_UNLOAD_SPRX:
            Proc_UnloadSPRX(Socket, Packet->Proc_SPRX.hModule, Packet->Proc_SPRX.Flags);
            break;

        case API_PROC_UNLOAD_SPRX_NAME:
            Proc_UnloadSPRX(Socket, Packet->Proc_SPRX.ModuleName, Packet->Proc_SPRX.Flags);
            break;

        case API_PROC_RELOAD_SPRX_NAME:
            Proc_ReloadSPRX(Socket, Packet->Proc_SPRX.ModuleName);
            break;

        case API_PROC_RELOAD_SPRX_HANDLE:
            Proc_ReloadSPRX(Socket, Packet->Proc_SPRX.hModule);
            break;

        case API_PROC_DUMP_MODULE:
            Proc_DumpModule(Socket, Packet->Proc_SPRX.ModuleName);
            break;

        case API_PROC_MODULE_LIST:
            Proc_GetModuleList(Socket);
            break;
    }
}