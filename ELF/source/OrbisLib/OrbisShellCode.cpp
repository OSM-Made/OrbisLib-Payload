#include "../Main.hpp"
#include "OrbisShellCode.hpp"
#include "../Util/Proc.hpp"

extern uint8_t OrbisProcHelper[];
extern int32_t OrbisProcHelperSize;

OrbisShellCode::OrbisShellCode()
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");

}

OrbisShellCode::~OrbisShellCode()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");
    
}

void OrbisShellCode::InstallShellCode(char* ProcName)
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

    gShellCodeSize = OrbisProcHelperSize;
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

    err = proc_rw_mem(proc, gShellCodePtr, OrbisProcHelperSize, (void *)OrbisProcHelper, &n, 1);
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

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, thr_initial), sizeof(thr_initial), (void *)&thr_initial, &n, 1);
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
	uint64_t ShellCodeEntry = (uint64_t)gShellCodePtr + *(uint64_t *)(OrbisProcHelper + 4);
	create_thread(thr, NULL, (void*)ShellCodeEntry, NULL, (char*)gStackPtr, StackSize, NULL, NULL, NULL, 0, NULL);
    
    ShellCodeLoaded = true;
}

void OrbisShellCode::DestroyShellCode()
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

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, ShouldExit), sizeof(ShouldExit), (void *)&ShouldExit, &n, 1);
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

int OrbisShellCode::sceKernelLoadStartModule(const char *name, size_t argc, const void *argv, unsigned int flags, int pOpt, int pRes)
{
    proc* proc = proc_find_by_name(ProcName);
	size_t n = 0;
    int err = 0;
	uint8_t CommandIndex = CMD_sceKernelLoadStartModule;
    uint8_t ShellCodeComplete = 0;
    uint64_t ModuleHandle = 0;

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, SPRXPath), strlen(name), (void *)name, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, argc), sizeof(argc), (void *)&argc, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, argv), sizeof(argv), (void *)&argv, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, flags), sizeof(flags), (void *)&flags, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, pOpt), sizeof(pOpt), (void *)&pOpt, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, pRes), sizeof(pRes), (void *)&pRes, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write params to ShellCode.");
        return 0;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, ModuleHandle), sizeof(ModuleHandle), (void *)&ModuleHandle, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to reset ModuleHandle to zero.");
        return 0;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, ShellCodeComplete), sizeof(ShellCodeComplete), (void *)&ShellCodeComplete, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to set ShellCodeComplete to zero.");
        return 0;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, CommandIndex), sizeof(CommandIndex), (void *)&CommandIndex, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to set CommandIndex.");
        return 0;
    }

	while (!ShellCodeComplete) 
	{
        err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, ShellCodeComplete), sizeof(ShellCodeComplete), (void *)&ShellCodeComplete, &n, 0);
        if(err)
        {
            DebugLog(LOGTYPE_ERR, "Failed to read ShellCodeComplete.");
            return 0;
        }

        //DebugLog(LOGTYPE_INFO, "Waiting for ShellCode to compelete!");
        pause("", 100);
	}

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, ModuleHandle), sizeof(ModuleHandle), (void *)&ModuleHandle, &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read ModuleHandle.");
        return 0;
    }

	return (int)(ModuleHandle);
}

int OrbisShellCode::sceKernelStopUnloadModule(int handle, size_t args, const void *argp, uint32_t flags, int* pOpt, int* pRes)
{
    proc* proc = proc_find_by_name(ProcName);
	size_t n = 0;
    int err = 0;
	uint8_t CommandIndex = CMD_sceKernelStopUnloadModule;
    uint8_t ShellCodeComplete = 0;
    uint64_t Result = 0;

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, handle), sizeof(handle), (void *)&handle, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, args), sizeof(args), (void *)&args, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, argp), sizeof(argp), (void *)&argp, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, flags), sizeof(flags), (void *)&flags, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, pOpt), sizeof(pOpt), (void *)&pOpt, &n, 1);
    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, pRes), sizeof(pRes), (void *)&pRes, &n, 1);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to write params to ShellCode.");
        return 0;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, Result), sizeof(Result), (void *)&Result, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to reset Result to zero.");
        return 0;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, ShellCodeComplete), sizeof(ShellCodeComplete), (void *)&ShellCodeComplete, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to set ShellCodeComplete to zero.");
        return 0;
    }

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, CommandIndex), sizeof(CommandIndex), (void *)&CommandIndex, &n, 1);
	if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to set CommandIndex.");
        return 0;
    }

	while (!ShellCodeComplete) 
	{
        err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, ShellCodeComplete), sizeof(ShellCodeComplete), (void *)&ShellCodeComplete, &n, 0);
        if(err)
        {
            DebugLog(LOGTYPE_ERR, "Failed to read ShellCodeComplete.");
            return 0;
        }

        //DebugLog(LOGTYPE_INFO, "Waiting for ShellCode to compelete!");
        pause("", 100);
	}

    err = proc_rw_mem(proc, gShellCodePtr + offsetof(OrbisProcHelper_header, Result), sizeof(Result), (void *)&Result, &n, 0);
    if(err)
    {
        DebugLog(LOGTYPE_ERR, "Failed to read Result.");
        return 0;
    }

	return (int)(Result);
}