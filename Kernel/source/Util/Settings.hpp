#pragma once

#define ORBISLIB_MAJOR_VERSION 2
#define ORBISLIB_MINOR_VERSION 4

//Which software version we want to compile for
#ifdef VERSION_505
#define SOFTWARE_VERSION_505
#endif
#ifdef VERSION_672
#define SOFTWARE_VERSION_672
#endif
#ifdef VERSION_702
#define SOFTWARE_VERSION_702
#endif
#ifdef VERSION_755
#define SOFTWARE_VERSION_755
#endif
