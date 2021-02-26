#include "../../Main.hpp"
#include "HelperManager.hpp"

HelperManager::HelperManager(/* args */)
{
    DebugLog(LOGTYPE_INFO, "Initialization...");

    //Used for loading Fake Selfs and SPRX
    this->pFakeSelfHelper = new FakeSelfHelper();

    //Highjack the Remoteplay Process for FTP
    LoadFTP("SceRemotePlay");
    //LoadFTP("SceShellCore");

    //Use ShellCore for our userland calls. Could be useful later for patches too 
    //                  and as ive found the SystemStateMgr could be used to control 
    //                  the power state of the system.
    //this->pUserlandHelper = new UserlandHelper();
    //this->pUserlandHelper->InstallShellCode("SceShellCore");
}

HelperManager::~HelperManager()
{
    DebugLog(LOGTYPE_INFO, "Destruction...");
    
    if(this->pFakeSelfHelper != NULL)
        delete this->pFakeSelfHelper;
    
    if(this->pUserlandHelper != NULL)
    {
        this->pUserlandHelper->DestroyShellCode();
        delete this->pUserlandHelper;
    }
}