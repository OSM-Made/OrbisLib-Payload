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

702:	
	@/bin/echo -e clear

	+$(MAKE) -C FTP clean
	+$(MAKE) -C FTP
	+$(MAKE) -C ShellCode clean
	+$(MAKE) -C ShellCode
	+$(MAKE) -C Kernel clean
	+$(MAKE) -C Kernel VERSION=702
	+$(MAKE) -C Userland clean
	+$(MAKE) -C Userland VERSION=702

	objcopy -O binary Userland/OrbisLib.bin OrbisLib-702.bin
	
	@/bin/echo -e "\e[1m\e[32m[7.02 Build Sucess!]\033[37m"

755:	
	@/bin/echo -e clear

	+$(MAKE) -C FTP clean
	+$(MAKE) -C FTP
	+$(MAKE) -C ShellCode clean
	+$(MAKE) -C ShellCode
	+$(MAKE) -C Kernel clean
	+$(MAKE) -C Kernel VERSION=755
	+$(MAKE) -C Userland clean
	+$(MAKE) -C Userland VERSION=755

	objcopy -O binary Userland/OrbisLib.bin OrbisLib-755.bin
	
	@/bin/echo -e "\e[1m\e[32m[7.55 Build Sucess!]\033[37m"

all:
	+$(MAKE) 505
	+$(MAKE) 672
	+$(MAKE) 702
	+$(MAKE) 755

	@/bin/echo -e "\e[1m\e[32m[All Build Sucess!]\033[37m"
