#pragma once

#include "Resolver-505.hpp"
#include "Resolver-672.hpp"

extern uint8_t* gKernelBase;

#ifndef resolve
#define resolve(x) ((void*)((uint8_t *)&gKernelBase[x]))
#endif