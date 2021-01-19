#pragma once

class OrbisShellCode
{
private:
    char ProcName[0x20] = { };
    int ProcessID = 0;

    void* gShellCodePtr = NULL;
    void* gStackPtr = NULL;
    size_t gShellCodeSize = 0;
    bool ShellCodeLoaded = false;

    struct OrbisProcHelper_header
    {
        uint32_t magic;
	    uint64_t entry;

        uint64_t thr_initial;
        uint8_t ShellCodeComplete;
	    uint8_t CommandIndex;
        uint8_t ShouldExit;

        char SPRXPath[100];
        int32_t argc;
        uint64_t argv; 
        int32_t flags;
        int32_t pOpt;
        int32_t pRes;
        uint64_t ModuleHandle;

        uint64_t args;
        uint64_t argp;
        int32_t handle;
        uint64_t Result;

        uint64_t sceKernelUsleep;
        uint64_t asceKernelLoadStartModule;
        uint64_t asceKernelStopUnloadModule;
    }__attribute__((packed));

    enum ShellCodeCommands
    {
        CMD_NA = 0,
        CMD_sceKernelLoadStartModule,
        CMD_sceKernelStopUnloadModule,
    };

public:
    OrbisShellCode();
    ~OrbisShellCode();

    void InstallShellCode(char* ProcName);
    void DestroyShellCode();

    int sceKernelLoadStartModule(const char *name, size_t argc, const void *argv, unsigned int flags, int pOpt, int pRes);
    int sceKernelStopUnloadModule(int handle, size_t args, const void *argp, uint32_t flags, int* pOpt, int* pRes);

};
