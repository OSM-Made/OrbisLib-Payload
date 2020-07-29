.section .rodata

    .global OrbisLibElf
    .type   OrbisLibElf, @object
    .align  4
    .global OrbisHen
	.type   OrbisHen, @object
	.align  4

OrbisLibElf:
    .incbin "../ELF/OrbisLib.elf"

OrbisLibElfEnd:
    .global OrbisLibElfSize
    .type   OrbisLibElfSize, @object
    .align  4

OrbisLibElfSize:
    .int    OrbisLibElfEnd - OrbisLibElf

OrbisHen:
    .incbin "../HEN/OrbisHen.bin"

OrbisHenEnd:
    .global OrbisHenSize
    .type   OrbisHenSize, @object
    .align  4

OrbisHenSize:
    .int    OrbisHenSize - OrbisHen
    