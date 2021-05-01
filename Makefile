505:	
	@/bin/echo -e clear

	+$(MAKE) -C FTP clean
	+$(MAKE) -C FTP
	+$(MAKE) -C ShellCode clean
	+$(MAKE) -C ShellCode
	+$(MAKE) -C Kernel clean
	+$(MAKE) -C Kernel VERSION=505
	+$(MAKE) -C Userland clean
	+$(MAKE) -C Userland VERSION=505

	objcopy -O binary Userland/OrbisLib.bin OrbisLib-505.bin
	
	@/bin/echo -e "\e[1m\e[32m[5.05 Build Sucess!]\033[37m"

672:	
	@/bin/echo -e clear

	+$(MAKE) -C FTP clean
	+$(MAKE) -C FTP
	+$(MAKE) -C ShellCode clean
	+$(MAKE) -C ShellCode
	+$(MAKE) -C Kernel clean
	+$(MAKE) -C Kernel VERSION=672
	+$(MAKE) -C Userland clean
	+$(MAKE) -C Userland VERSION=672

	objcopy -O binary Userland/OrbisLib.bin OrbisLib-672.bin
	
	@/bin/echo -e "\e[1m\e[32m[6.72 Build Sucess!]\033[37m"