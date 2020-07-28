#pragma once

class OrbisShellCode
{
private:
    char ProcName[0x20] = { };
    int ProcessID = 0;

public:
    OrbisShellCode(const char* ProcName, int ProcID);
    ~OrbisShellCode();

    void InstallShellCode();
    void DestroyShellCode();
};
