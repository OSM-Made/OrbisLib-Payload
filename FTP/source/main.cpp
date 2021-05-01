#include "../include/main.hpp"
#include "../include/FTP.hpp"
#include "../include/memory.hpp"

extern "C" void _main(void *arg) {
	LoadImports();
	InitFTP();
}