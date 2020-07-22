#include <stdint.h>
#include <stdarg.h>

#include "../include/FreeBSD.h"

extern "C" void _main(void) 
{
	net_disable_copy_checks();
	printf("Hello from kernel ELF CPP!\n");
}
