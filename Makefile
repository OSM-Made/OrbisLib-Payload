all:	
	clear

	+$(MAKE) -C FTP clean
	+$(MAKE) -C FTP
	+$(MAKE) -C ShellCode clean
	+$(MAKE) -C ShellCode
	+$(MAKE) -C ELF clean
	+$(MAKE) -C ELF
	+$(MAKE) -C Payload clean
	+$(MAKE) -C Payload

	#objcopy -O binary Payload/OrbisLib.bin OrbisLib.bin
	
	@/bin/echo -e "\e[1m\e[32m[Build Sucess!]"