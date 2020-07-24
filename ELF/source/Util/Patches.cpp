#include "../Main.hpp"
#include "Patches.hpp"

void Install_Patches()
{
    gKernelBase = (uint8_t*)__readmsr(0xC0000082) - addr_Xfast_syscall;

    if (!gKernelBase) {
        //printf("oh fuk kernel go boom!\n");
        return;
    }

    //TODO: Make patches	
}