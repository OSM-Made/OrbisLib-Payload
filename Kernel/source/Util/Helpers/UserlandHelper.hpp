#pragma once

class UserlandHelper
{
private:
    char ProcName[0x20] = { };
    int ProcessID = 0;

    void* gShellCodePtr = NULL;
    void* gStackPtr = NULL;
    size_t gShellCodeSize = 0;
    bool ShellCodeLoaded = false;

    struct OrbisUserlandHelper_header
    {
        uint32_t magic;
	    uint64_t entry;

        uint64_t thr_initial;
        uint64_t SceSysCoreBase;
        uint64_t spawnProcess;
        uint32_t Result;
        int32_t Complete;

        uint64_t param3;
        uint64_t param4;

        int32_t ProcessID;
    }__attribute__((packed));

public:
    UserlandHelper(/* args */);
    ~UserlandHelper();

    void InstallShellCode(char* ProcName);
    void DestroyShellCode();

};
