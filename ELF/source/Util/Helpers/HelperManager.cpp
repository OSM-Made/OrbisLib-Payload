#include "../../Main.hpp"
#include "HelperManager.hpp"

HelperManager::HelperManager(/* args */)
{
    DebugLog(LOGTYPE_INFO, "Initialization...");

    this->pFakeSelfHelper = new FakeSelfHelper();

    //LoadFTP("SceRemotePlay");

    this->pUserlandHelper = new UserlandHelper();
    this->pUserlandHelper->InstallShellCode("SceRemotePlay");

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