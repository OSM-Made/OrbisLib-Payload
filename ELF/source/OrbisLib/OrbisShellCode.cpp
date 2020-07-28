#include "../Main.hpp"
#include "OrbisShellCode.hpp"

OrbisShellCode::OrbisShellCode(const char* ProcName, int ProcID)
{
    DebugLog(LOGTYPE_INFO, "Initialization!!");

    //install shell code
    InstallShellCode();
}

OrbisShellCode::~OrbisShellCode()
{
    DebugLog(LOGTYPE_INFO, "Destruction!!");
    
}

void OrbisShellCode::InstallShellCode()
{
    
}

void OrbisShellCode::DestroyShellCode()
{

}

