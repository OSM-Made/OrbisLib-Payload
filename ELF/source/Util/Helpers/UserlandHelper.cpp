#include "../../Main.hpp"
#include "UserlandHelper.hpp"
#include "../Proc.hpp"

extern uint8_t OrbisUserlandHelper[];
extern int32_t OrbisUserlandHelperSize;

UserlandHelper::UserlandHelper(/* args */)
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");
}

UserlandHelper::~UserlandHelper()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");
}

void UserlandHelper::InstallShellCode(char* ProcName)
{
    size_t n;
    int err = 0;
    proc* proc = proc_find_by_name(ProcName); 
    uint64_t thr_initial = 0;

    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", ProcName);
        return;
    }

    //store for use later
    strcpy(this->ProcName, ProcName);

    gShellCodeSize = OrbisUserlandHelperSize;
	gShellCodeSize += (PAGE_SIZE - (gShellCodeSize % PAGE_SIZE));
	if(proc_allocate(proc, &gShellCodePtr, gShellCodeSize)) 
    {
        DebugLog(LOGTYPE_ERR, "Failed to allocate ShellCode Memory.");
        return;
    }

    size_t StackSize = 0x80000;
	if(proc_allocate(proc, &gStackPtr, StackSize)) {
        DebugLog(LOGTYPE_ERR, "Failed to allocate Stack Memory.");

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

        return;
    }

    err = proc_rw_mem(proc, gShellCodePtr, OrbisUserlandHelperSize, (void *)OrbisUserlandHelper, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write Shellcode to Memory. Error: %d.", err);

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return;
    }

    if(proc->p_dynlibptr == NULL) 
    {
        DebugLog(LOGTYPE_ERR, "p_dynlibptr is NULL.");

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return;
    }

    dynlib* m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{
        if(!strcmp(basename(m_library->ModulePath), "libkernel.sprx"))
			thr_initial = (uint64_t)m_library->codeBase + 0x84C20;

        if(!strcmp(basename(m_library->ModulePath), "libkernel_web.sprx"))
			thr_initial = (uint64_t)m_library->codeBase + 0x84C20;

        if(!strcmp(basename(m_library->ModulePath), "libkernel_sys.sprx"))
			thr_initial = (uint64_t)m_library->codeBase + 0x89030;

        m_library = m_library->dynlib_next;
    }

    if(thr_initial == 0) 
    {
		DebugLog(LOGTYPE_ERR, "Failed to resolve thr_initial.");

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return;
    }

    //Set Text Segments as writeable.
    m_library = proc->p_dynlibptr->p_dynlib;
    while(m_library != 0)
	{ 
        proc_mprotect(proc, (void *)m_library->codeBase, (void*)m_library->codeSize, VM_PROT_ALL);
        m_library = m_library->dynlib_next;
    }

    //DebugLog(LOGTYPE_INFO, "thr_initial = %llX", thr_initial);
    //DebugLog(LOGTYPE_INFO, "gShellCodePtr = %llX", gShellCodePtr);

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, thr_initial), sizeof(thr_initial), (void *)&thr_initial, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write thr_initial to ShellCode. Error: %d.", err);

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return;
    }

    struct thread *thr = TAILQ_FIRST(&proc->p_threads);
	uint64_t ShellCodeEntry = (uint64_t)gShellCodePtr + *(uint64_t *)(OrbisUserlandHelper + 4);
	create_thread(thr, NULL, (void*)ShellCodeEntry, NULL, (char*)gStackPtr, StackSize, NULL, NULL, NULL, 0, NULL);
    
    ShellCodeLoaded = true;
}

void UserlandHelper::DestroyShellCode()
{
    proc* proc = proc_find_by_name(ProcName);
    int err = 0;
    uint8_t ShouldExit = 1;
    size_t n;

    if(!ShellCodeLoaded)
    {
        DebugLog(LOGTYPE_INFO, "ShellCode not loaded.");

        return;
    }

    if(!proc)
    {
        DebugLog(LOGTYPE_ERR, "Could not find Proc \"%s\".", ProcName);

        gShellCodePtr = NULL;
	    gStackPtr = NULL;
        ShellCodeLoaded = false;

        return;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, ShouldExit), sizeof(ShouldExit), (void *)&ShouldExit, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to set ShouldExit to 1.");
        return;
    }

    pause("", 500);

    if (gShellCodePtr)
        proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

    if (gStackPtr)
        proc_deallocate(proc, gStackPtr, 0x80000);

    gShellCodePtr = NULL;
    gStackPtr = NULL;
    ShellCodeLoaded = false;
}

void UserlandHelper::sceSysUtilSendNotificationRequest(const char* Message)
{
    proc* proc = proc_find_by_name(ProcName);
	size_t n = 0;
    int err = 0;
	uint8_t CommandIndex = CMD_sceSysUtilSendNotificationRequest;
    uint8_t ShellCodeComplete = 0;
    uint64_t ModuleHandle = 0;

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, Message), 100, (void *)Message, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write params to ShellCode.");
        return;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, ShellCodeComplete), sizeof(ShellCodeComplete), (void *)&ShellCodeComplete, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to set ShellCodeComplete to zero.");
        return;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, CommandIndex), sizeof(CommandIndex), (void *)&CommandIndex, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to set CommandIndex.");
        return;
    }

	while (!ShellCodeComplete) 
	{
        err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, ShellCodeComplete), sizeof(ShellCodeComplete), (void *)&ShellCodeComplete, &n, 0);
        if(err)
        {
            DebugLog(LOGTYPE_ERR, "Failed to read ShellCodeComplete.");
            return;
        }

        //DebugLog(LOGTYPE_INFO, "Waiting for ShellCode to compelete!");
        pause("", 100);
	}
}

void UserlandHelper::sceSysUtilSendSystemNotificationWithText(int MessageType, const char* Message)
{
    proc* proc = proc_find_by_name(ProcName);
	size_t n = 0;
    int err = 0;
	uint8_t CommandIndex = CMD_sceSysUtilSendSystemNotificationWithText;
    uint8_t ShellCodeComplete = 0;
    uint64_t ModuleHandle = 0;

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, Message), 100, (void *)Message, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, MessageType), sizeof(MessageType), (void *)&MessageType, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write params to ShellCode.");
        return;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, ShellCodeComplete), sizeof(ShellCodeComplete), (void *)&ShellCodeComplete, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to set ShellCodeComplete to zero.");
        return;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, CommandIndex), sizeof(CommandIndex), (void *)&CommandIndex, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to set CommandIndex.");
        return;
    }

	while (!ShellCodeComplete) 
	{
        err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, ShellCodeComplete), sizeof(ShellCodeComplete), (void *)&ShellCodeComplete, &n, 0);
        if(err)
        {
            DebugLog(LOGTYPE_ERR, "Failed to read ShellCodeComplete.");
            return;
        }

        pause("", 100);
	}
}