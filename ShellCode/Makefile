ShellCode = OrbisProcHelper.bin
ShellCodes = OrbisProcHelper.s
ShellCode2 = OrbisELFLoader.bin
ShellCode2s = OrbisELFLoader.s


all: clean $(ShellCode)
all: clean $(ShellCode2)

$(ShellCode):
	nasm -f bin -o $(ShellCode) $(ShellCodes)

$(ShellCode2):
	nasm -f bin -o $(ShellCode2) $(ShellCode2s)
	
.PHONY: clean
clean:
	rm -f $(ShellCode) 
	rm -f $(ShellCode2)