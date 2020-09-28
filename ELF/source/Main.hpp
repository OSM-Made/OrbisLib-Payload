#pragma once

#include <stdint.h>
#include <stdarg.h>

#include "Util/Types.hpp"
#include "Util/Settings.hpp"
#include "OrbisLib/OrbisDef.hpp"
#include "Util/Utilites.hpp"
#include "Util/Heap.hpp"
#include "Util/Kernel.hpp"
#include "Util/Resolver/Resolver.hpp"
#include "Util/Resolver/Patches.hpp"
#include "Util/Proc.hpp"
#include "Util/Network.hpp"
#include "Util/DebugLogger.hpp"
#include "Util/ELFLoader.hpp"
#include "OrbisLib/OrbisLib.hpp"
#include "Util/Helpers/HelperManager.hpp"

extern "C"
{
    
    #include <sys/lock.h>
    #include <sys/mutex.h>

	#include <sys/proc.h>
    #include <sys/sysent.h>
    #include <sys/syscall.h>
    #include <sys/ptrace.h>
    #include <sys/uio.h>
    #include <sys/mman.h>
    #include <sys/pcpu.h>
    #include <vm/vm.h>
    #include <vm/pmap.h>
    #include <vm/vm_map.h>

    #include <errno.h>
    #include <sys/elf_common.h>
    #include <sys/elf64.h>
    #include <sys/eventhandler.h>
    #include <wait.h>
    #include <machine/reg.h>
    #include <fcntl.h>
    #include <time.h>
    #include <dirent.h>

    #include <sys/stat.h>
    #include "sys/mount.h"
}

extern OrbisLib* orbisLib;
extern HelperManager* pHelperManager;
