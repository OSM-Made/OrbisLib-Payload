#pragma once

#define NULL 0

#define FALSE 0
#define TRUE 1

#define PAGE_SIZE (16 * 1024)

#include <stdint.h>
#include <stdarg.h>

#include "types.h"
#include "Offsets.h"
#include "AMD.h"
#include "freebsd.h"
#include "syscall.h"
#include "Lib.h"
#include "resolve.h"
#include "elf.h"