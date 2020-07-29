BITS 64
DEFAULT REL

magic: db 'SHEL'
entry: dq shellcode

thr_initial: dq 0
ShellCodeComplete: db 0
CommandIndex: db 0
ShouldExit: db 0

;sceKernelLoadStartModule Variables
ModulePath: db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
argc: dd 0
argv: dq 0
flags: dd 0
pOpt: dd 0
pRes: dd 0
ModuleHandle: dq 0

;sceKernelStopUnloadModule Variables
args: dq 0
argp: dq 0
handle: dd 0
Result: dq 0


;Addresses
libkernel: dq 0
str_libkernel: db 'libkernel.sprx', 0
str_libkernelweb: db 'libkernel_web.sprx', 0
str_libkernelsys: db 'libkernel_sys.sprx', 0
sceKernelUsleep: dq 0
str_sceKernelSleep: db 'sceKernelUsleep', 0
asceKernelLoadStartModule: dq 0
str_sceKernelLoadStartModule: db 'sceKernelLoadStartModule', 0
asceKernelStopUnloadModule: dq 0
str_sceKernelStopUnloadModule: db 'sceKernelStopUnloadModule', 0

shellcode:
	; load thr_initial into fs
	mov rdi, qword [thr_initial]
	mov rsi, qword [rdi]
	mov rdi, qword [rsi + 0x1E0]
	call amd64_set_fsbase

	; get libkernel handle
	mov rcx, 0
	lea rdx, [libkernel]
	mov rsi, 0
	lea rdi, [str_libkernel]
	call sys_dynlib_load_prx
	test rax, rax
	je resolve

	mov rcx, 0
	lea rdx, [libkernel]
	mov rsi, 0
	lea rdi, [str_libkernelweb]
	call sys_dynlib_load_prx
	test rax, rax
	je resolve

	mov rcx, 0
	lea rdx, [libkernel]
	mov rsi, 0
	lea rdi, [str_libkernelsys]
	call sys_dynlib_load_prx

resolve:
	; resolve sceKernelUsleep
	lea rdx, [sceKernelUsleep]
	lea rsi, [str_sceKernelSleep]
	mov rdi, qword [libkernel]
	call sys_dynlib_dlsym

	; resolve sceKernelLoadStartModule
	lea rdx, [asceKernelLoadStartModule]
	lea rsi, [str_sceKernelLoadStartModule]
	mov rdi, qword [libkernel]
	call sys_dynlib_dlsym

	; resolve sceKernelStopUnloadModule
	lea rdx, [asceKernelStopUnloadModule]
	lea rsi, [str_sceKernelStopUnloadModule]
	mov rdi, qword [libkernel]
	call sys_dynlib_dlsym

LoopStart:
	cmp byte[ShouldExit], 1
	je LoopExit
	
FirstCall:
	cmp byte[CommandIndex], 1
	jne SecondCall

	mov byte [ShellCodeComplete], 0
	call sceKernelLoadStartModule
    mov byte [ShellCodeComplete], 1

	jmp EndofCase
SecondCall:
	cmp byte[CommandIndex], 2
	jne EndofCase
	
	mov byte [ShellCodeComplete], 0
	call sceKernelStopUnloadModule
    mov byte [ShellCodeComplete], 1

	jmp EndofCase

EndofCase:
	mov rdi, 100000
	mov r12, qword [sceKernelUsleep]
	call r12

	mov byte[CommandIndex], 0
	jmp LoopStart

LoopExit:
	mov rdi, 0
	call sys_thr_exit
	retn



sceKernelStopUnloadModule:
	mov rdi, [handle]
 	mov rsi, qword [args]
	mov rdx, qword [argp]
	mov rcx, [flags]
	mov r8, qword [pOpt]
	mov r9, qword [pRes]
	mov r12, qword [asceKernelStopUnloadModule]
	call r12
	mov qword [Result], rax
	xor eax, eax
	ret

sceKernelLoadStartModule:
	mov rdi, qword [ModulePath]
	mov rsi, [argc]
	mov rdx, qword [argv]
	mov rcx, [flags]
	mov r8, [pOpt]
	mov r9, [pRes]
	mov r12, qword [asceKernelLoadStartModule]
	call r12
	mov qword [ModuleHandle], rax
	xor eax, eax
	ret

sys_dynlib_load_prx:
	mov rax, 594
	mov r10, rcx
	syscall
	retn

sys_dynlib_dlsym:
	mov rax, 591
	mov r10, rcx
	syscall
	retn

sys_thr_exit:
	mov rax, 431
	mov r10, rcx
	syscall
	retn

sys_sysarch:
	mov rax, 165
	mov r10, rcx
	syscall
	retn

amd64_set_fsbase:
	push rbp
	mov rbp, rsp
	push rbx
	sub rsp, 0x18

	mov [rbp - 0x18], rdi

	lea rsi, [rbp - 0x18]
	mov edi, 129
	call sys_sysarch

	add rsp, 0x18
	pop rbx
	pop rbp
	retn