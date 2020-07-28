#pragma once

#include <stdint.h>
#include <stdarg.h>

#include "Util/Types.hpp"
#include "Util/Settings.hpp"
#include "Util/Heap.hpp"
#include "Util/Kernel.hpp"
#include "Util/Resolver/Resolver.hpp"
#include "Util/Resolver/Patches.hpp"
#include "Util/Network.hpp"
#include "Util/Debug-Logger.hpp"
#include "OrbisLib/OrbisLib.hpp"

extern "C"
{
    #include <sys/lock.h>
    #include <sys/mutex.h>

	#include <sys/proc.h>
    #include <sys/sysent.h>
    #include <sys/syscall.h>
    #include <sys/ptrace.h>
    #include <sys/uio.h>
	#include <sys/mount.h>
    #include <sys/mman.h>

    #include <vm/vm.h>
    #include <vm/pmap.h>
    #include <vm/vm_map.h>
}