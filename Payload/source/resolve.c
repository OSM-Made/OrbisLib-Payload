#include "../include/main.h"

#define RESOLVE(name, base, offset) \
	name = (void *)(base + offset) \

void resolve(uint64_t kernbase) {
	RESOLVE(M_TEMP, kernbase, __M_TEMP);
	RESOLVE(malloc, kernbase, __malloc);
	RESOLVE(free, kernbase, __free);
	RESOLVE(memcpy, kernbase, __memcpy);
	RESOLVE(memset, kernbase, __memset);
	RESOLVE(memcmp, kernbase, __memcmp);
	RESOLVE(strlen, kernbase, __strlen);
	RESOLVE(vsprintf, kernbase, __vsprintf);
	RESOLVE(sprintf, kernbase, __sprintf);
}
