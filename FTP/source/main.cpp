#include "../include/main.h"
#include "../include/FTP.h"

extern "C" void _main(void *arg) {
	LoadImports();

	InitFTP();
}