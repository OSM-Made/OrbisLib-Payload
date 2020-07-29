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
    