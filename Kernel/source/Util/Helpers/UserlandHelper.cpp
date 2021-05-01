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
    uint64_t SceSysCoreBase = 0;

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
			thr_initial = (uint64_t)m_library->codeBase + addr_thr_initial_libkernel;

        if(!strcmp(basename(m_library->ModulePath), "libkernel_web.sprx"))
			thr_initial = (uint64_t)m_library->codeBase + addr_thr_initial_libkernel_web;

        if(!strcmp(basename(m_library->ModulePath), "libkernel_sys.sprx"))
			thr_initial = (uint64_t)m_library->codeBase + addr_thr_initial_libkernel_sys;

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
        if(!strcmp(basename(m_library->ModulePath), "SceSysCore.elf"))
            SceSysCoreBase = m_library->codeBase;

        proc_mprotect(proc, (void *)m_library->codeBase, (void*)m_library->codeSize, VM_PROT_ALL);
        m_library = m_library->dynlib_next;
    }

    DebugLog(LOGTYPE_INFO, "SceSysCoreBase = %llX", SceSysCoreBase);
    DebugLog(LOGTYPE_INFO, "thr_initial = %llX", thr_initial);
    DebugLog(LOGTYPE_INFO, "gShellCodePtr = %llX", gShellCodePtr);

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

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, SceSysCoreBase), sizeof(SceSysCoreBase), (void *)&SceSysCoreBase, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write SceSysCoreBase to ShellCode. Error: %d.", err);

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return;
    }

    uint64_t spawnProcess = SceSysCoreBase + 0x9EA0;//0x3B40;
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, spawnProcess), sizeof(spawnProcess), (void *)&spawnProcess, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write spawnProcess to ShellCode. Error: %d.", err);

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return;
    }

    uint64_t param3 = SceSysCoreBase + 0xD0120;
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, param3), sizeof(param3), (void *)&param3, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write param3 to ShellCode. Error: %d.", err);

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return;
    }

    uint64_t param4 = SceSysCoreBase + 0x96EE0;
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, param4), sizeof(param4), (void *)&param4, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write param4 to ShellCode. Error: %d.", err);

        if (gShellCodePtr)
			proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

		if (gStackPtr)
			proc_deallocate(proc, gStackPtr, 0x80000);

        return;
    }

    struct thread *thr = TAILQ_FIRST(&proc->p_threads);
	uint64_t ShellCodeEntry = (uint64_t)gShellCodePtr + *(uint64_t *)(OrbisUserlandHelper + 4);
	create_thread(thr, NULL, (void*)ShellCodeEntry, NULL, (char*)gStackPtr, StackSize, NULL, NULL, NULL, 0, NULL);

    int32_t Complete = 0;
    uint32_t Result = 0;
    int32_t ProcessID = 0;

    while (!Complete) 
	{
        err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, Complete), sizeof(Complete), (void *)&Complete, &n, 0);
        if(err)
        {
            DebugLog(LOGTYPE_ERR, "Failed to read Complete.");
            return;
        }
        pause("", 100);
	}

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, Result), sizeof(Result), (void *)&Result, &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read Result.");
        return;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisUserlandHelper_header, ProcessID), sizeof(ProcessID), (void *)&ProcessID, &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read Result.");
        return;
    }

    printf("ProcessID = %i\nResult = %X\n", ProcessID, Result);
    
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

    pause("", 500);

    if (gShellCodePtr)
        proc_deallocate(proc, gShellCodePtr, gShellCodeSize);

    if (gStackPtr)
        proc_deallocate(proc, gStackPtr, 0x80000);

    gShellCodePtr = NULL;
    gStackPtr = NULL;
    ShellCodeLoaded = false;
}