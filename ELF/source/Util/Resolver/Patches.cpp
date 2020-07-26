#include "../../Main.hpp"
#include "Patches.hpp"
#include "Resolver.hpp"
#include "../kernel.hpp"

void Install_Patches()
{
    gpKernelBase = (uint8_t*)__readmsr(0xC0000082) - addr_Xfast_syscall;

    if (!gpKernelBase) {
        //printf("Failed to get Kernel Base!\n");
        return;
    }

    //TODO: Make patches	
 	uint64_t CR0 = __readcr0();
 	__writecr0(CR0 & ~CR0_WP);

 	*(uint16_t*)resolve(addr_copyin1) = 0x9090;
	*(uint16_t*)resolve(addr_copyin2) = 0x9090;
	*(uint16_t*)resolve(addr_copyout1) = 0x9090;
	*(uint16_t*)resolve(addr_copyout2) = 0x9090;
	*(uint16_t*)resolve(addr_copyout3) = 0x9090;

 	__writecr0(CR0);
}