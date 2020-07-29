#include "../include/main.h"
#include "memory.h"

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
    syscall(477, addr, len, prot, flags, fd, offset);
}

int munmap(void *addr, size_t len)
{
    syscall(73, addr, len);
}

int mprotect(void *addr, size_t len, int prot)
{
    syscall(74, addr, len, prot);
}

int msync(void *addr, size_t len, int flags)
{
    syscall(65, addr, len, flags);
}

int mlock(void *addr, size_t len)
{
    syscall(203, addr, len);
}

int munlock(void *addr, size_t len)
{
    syscall(204, addr, len);
}

int getMemoryInfo(void *address, struct memoryRegionInfo *destination)
{
    syscall(547, address, destination);
}

int getOtherMemoryInfo(void *address, int nextMatchIfUnmapped, struct otherMemoryRegionInfo *destination)
{
    syscall(572, address, nextMatchIfUnmapped, destination);
}