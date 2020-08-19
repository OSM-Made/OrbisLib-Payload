.section .rodata

    .global OrbisLibElf
    .type   OrbisLibElf, @object
    .align  4

OrbisLibElf:
    .incbin "../ELF/OrbisLib.elf"

OrbisLibElfEnd:
    .global OrbisLibElfSize
    .type   OrbisLibElfSize, @object
    .align  4

OrbisLibElfSize:
    .int    OrbisLibElfEnd - OrbisLibElf
    