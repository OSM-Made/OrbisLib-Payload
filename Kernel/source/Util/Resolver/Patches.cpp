#include "../../Main.hpp"
#include "Patches.hpp"
#include "Resolver.hpp"
#include "../kernel.hpp"

void Install_Patches()
{
    gpKernelBase = (uint8_t*)__readmsr(0xC0000082) - addr_Xfast_syscall;

    if (!gpKernelBase) {
        kprintf("Failed to get Kernel Base!\n");
        return;
    }

 	uint64_t CR0 = __readcr0();
 	__writecr0(CR0 & ~CR0_WP);

 	*(uint16_t*)resolve(patch_copyin1) = 0x9090;
	*(uint16_t*)resolve(patch_copyin2) = 0x9090;
	*(uint16_t*)resolve(patch_copyout1) = 0x9090;
	*(uint16_t*)resolve(patch_copyout2) = 0x9090;
	*(uint16_t*)resolve(patch_copyout3) = 0x9090;

	//swword_lwpid check to see if child_tid/parent_tid is in kernel memory
	*(uint16_t*)resolve(patch_swword_lwpid_0) = 0x9090;
	*(uint16_t*)resolve(patch_swword_lwpid_1) = 0x9090;

	//Enable mount for unprivileged user
	*(uint16_t*)resolve(patch_mount) = 0x9090;
	*(uint16_t*)resolve(patch_mount + 0x2) = 0x9090;
	*(uint16_t*)resolve(patch_mount + 0x4) = 0x9090;

 	__writecr0(CR0);
}