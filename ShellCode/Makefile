ShellCode = OrbisProcHelper.bin
ShellCodes = OrbisProcHelper.s
ShellCode2 = OrbisELFLoader.bin
ShellCode2s = OrbisELFLoader.s
ShellCode3 = OrbisUserlandHelper.bin
ShellCode3s = OrbisUserlandHelper.s

all: clean $(ShellCode)
all: clean $(ShellCode2)
all: clean $(ShellCode3)

$(ShellCode):
	nasm -f bin -o $(ShellCode) $(ShellCodes)

$(ShellCode2):
	nasm -f bin -o $(ShellCode2) $(ShellCode2s)

$(ShellCode3):
	nasm -f bin -o $(ShellCode3) $(ShellCode3s)
	
.PHONY: clean
clean:
	rm -f $(ShellCode) 
	rm -f $(ShellCode2)
	rm -f $(ShellCode3)