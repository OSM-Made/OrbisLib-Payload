#pragma once
#include "FakeSelfHelper.hpp"
#include "FTPHelper.hpp"
#include "UserlandHelper.hpp"
#include "KLogHelper.hpp"

class HelperManager
{
private:
    
public:
    FakeSelfHelper* pFakeSelfHelper;
    UserlandHelper* pUserlandHelper;
    KLogHelper* pKLogHelper;

    HelperManager(/* args */);
    ~HelperManager();
};
