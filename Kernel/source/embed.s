.section .rodata
    .global OrbisProcHelper
	.type   OrbisProcHelper, @object
	.align  4

    .global OrbisELFLoader
	.type   OrbisELFLoader, @object
	.align  4

    .global OrbisUserlandHelper
	.type   OrbisUserlandHelper, @object
	.align  4

    .global OrbisFTP
	.type   OrbisFTP, @object
	.align  4

OrbisProcHelper:
    .incbin "../ShellCode/OrbisProcHelper.bin"
OrbisProcHelperEnd:
    .global OrbisProcHelperSize
    .type   OrbisProcHelperSize, @object
    .align  4
OrbisProcHelperSize:
    .int    OrbisProcHelperEnd - OrbisProcHelper

OrbisELFLoader:
    .incbin "../ShellCode/OrbisELFLoader.bin"
OrbisELFLoaderEnd:
    .global OrbisELFLoaderSize
    .type   OrbisELFLoaderSize, @object
    .align  4
OrbisELFLoaderSize:
    .int    OrbisELFLoaderEnd - OrbisELFLoader

OrbisUserlandHelper:
    .incbin "../ShellCode/OrbisUserlandHelper.bin"
OrbisUserlandHelperEnd:
    .global OrbisUserlandHelperSize
    .type   OrbisUserlandHelperSize, @object
    .align  4
OrbisUserlandHelperSize:
    .int    OrbisUserlandHelperEnd - OrbisUserlandHelper

OrbisFTP:
    .incbin "../FTP/OrbisFTP.elf"
OrbisFTPEnd:
    .global OrbisFTPSize
    .type   OrbisFTPSize, @object
    .align  4
OrbisFTPSize:
    .int    OrbisFTPEnd - OrbisFTP
    