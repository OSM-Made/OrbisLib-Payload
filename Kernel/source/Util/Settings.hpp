#pragma once

#define ORBISLIB_MAJOR_VERSION 2
#define ORBISLIB_MINOR_VERSION 5

//Which software version we want to compile for
#define SOFTWARE_VERSION_505
#ifdef VERSION_672
#define SOFTWARE_VERSION_672
#undef SOFTWARE_VERSION_505
#endif
#ifdef VERSION_702
#define SOFTWARE_VERSION_702
#undef SOFTWARE_VERSION_505
#endif
#ifdef VERSION_755
#define SOFTWARE_VERSION_755
#undef SOFTWARE_VERSION_505
#endif
