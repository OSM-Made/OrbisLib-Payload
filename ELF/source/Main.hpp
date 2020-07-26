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
	#include <sys/proc.h>
    #include <sys/sysent.h>
    #include <sys/syscall.h>
}