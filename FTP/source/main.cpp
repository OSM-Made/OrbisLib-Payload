#include "../include/main.hpp"
#include "../include/FTP.hpp"

extern "C" void _main(void *arg) {
	LoadImports();

	InitFTP();
}