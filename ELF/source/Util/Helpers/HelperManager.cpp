#include "../../Main.hpp"
#include "HelperManager.hpp"

HelperManager::HelperManager(/* args */)
{
    DebugLog(LOGTYPE_INFO, "Initialization...");

    this->pFakeSelfHelper = new FakeSelfHelper();
}

HelperManager::~HelperManager()
{
    DebugLog(LOGTYPE_INFO, "Destruction...");
    
    if(this->pFakeSelfHelper != NULL)
        delete this->pFakeSelfHelper;
    
}