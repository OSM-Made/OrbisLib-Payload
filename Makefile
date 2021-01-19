all:	
	clear

	+$(MAKE) -C FTP clean
	+$(MAKE) -C FTP
	+$(MAKE) -C ShellCode clean
	+$(MAKE) -C ShellCode
	+$(MAKE) -C Kernel clean
	+$(MAKE) -C Kernel
	+$(MAKE) -C Userland clean
	+$(MAKE) -C Userland

	objcopy -O binary Userland/OrbisLib.bin OrbisLib.bin
	
	@/bin/echo -e "\e[1m\e[32m[Build Sucess!]\033[37m"
