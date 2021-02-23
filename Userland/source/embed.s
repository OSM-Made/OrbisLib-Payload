.section .rodata

    .global OrbisLibElf
    .type   OrbisLibElf, @object
    .align  4

OrbisLibElf:
    .incbin "../Kernel/Kernel.elf"

OrbisLibElfEnd:
    .global OrbisLibElfSize
    .type   OrbisLibElfSize, @object
    .align  4

OrbisLibElfSize:
    .int    OrbisLibElfEnd - OrbisLibElf
    