#pragma once
#include "FakeSelfHelper.hpp"
#include "FTPHelper.hpp"
#include "UserlandHelper.hpp"

class HelperManager
{
private:
    
public:
    FakeSelfHelper* pFakeSelfHelper;
    UserlandHelper* pUserlandHelper;

    HelperManager(/* args */);
    ~HelperManager();
};
