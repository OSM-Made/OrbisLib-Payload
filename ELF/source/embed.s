.section .rodata

    .global OrbisShellCode
	.type   OrbisShellCode, @object
	.align  4

OrbisShellCode:
    .incbin "../ShellCode/ShellCode.bin"
OrbisShellCodeEnd:
    .global OrbisShellCodeSize
    .type   OrbisShellCodeSize, @object
    .align  4
OrbisShellCodeSize:
    .int    OrbisShellCodeEnd - OrbisShellCode
    