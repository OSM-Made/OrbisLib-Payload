.section .rodata
    .global OrbisProcHelper
	.type   OrbisProcHelper, @object
	.align  4

OrbisProcHelper:
    .incbin "../ShellCode/OrbisProcHelper.bin"
OrbisProcHelperEnd:
    .global OrbisProcHelperSize
    .type   OrbisProcHelperSize, @object
    .align  4
OrbisProcHelperSize:
    .int    OrbisProcHelperEnd - OrbisProcHelper

.section .rodata
    .global OrbisFTP
	.type   OrbisFTP, @object
	.align  4

OrbisFTP:
    .incbin "../FTP/OrbisFTP.elf"
OrbisFTPEnd:
    .global OrbisFTPSize
    .type   OrbisFTPSize, @object
    .align  4
OrbisFTPSize:
    .int    OrbisFTPEnd - OrbisFTP
    