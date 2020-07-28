BITS 64
DEFAULT REL

magic: db 'SHEL'
entry: dq shellcode

ShellCodeComplete: db 0
CommandIndex: db 0

;sceKernelLoadStartModule Variables
ModulePath: db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
argc: dq 0
argv: dq 0
flags: dd 0
pOpt: dd 0
pRes: dd 0
ModuleHandle: dq 0
asceKernelLoadStartModule: dq 0

;sceKernelStopUnloadModule Variables
args: dq 0
argp: dq 0
handle: dd 0
Result: dq 0
asceKernelStopUnloadModule: dq 0

;sceSysUtilSendSystemNotificationWithText Variables
messageType: dd 0
message: db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

;Addresses
pThread: dq 0

libSceSysUtil: dq 0
str_libSceSysUtil: db 'libSceSysUtil.sprx', 0
libkernel: dq 0
str_libkernel: db 'libkernel.sprx', 0
str_libkernelweb: db 'libkernel_web.sprx', 0
str_libkernelsys: db 'libkernel_sys.sprx', 0
str_sceSysUtilSendSystemNotificationWithText: db 'sceSysUtilSendSystemNotificationWithText', 0
asceSysUtilSendSystemNotificationWithText: dq 0
sceKernelUsleep: dq 0
str_sceKernelSleep: db 'sceKernelUsleep', 0

shellcode:
	; load pthread data into fs
	mov rdi, qword [pThread]
	mov rsi, qword [rdi]
	mov rdi, qword [rsi + 0x1E0]
	call amd64_set_fsbase

	; get libSceSysUtil handle
	mov rcx, 0
	lea rdx, [libSceSysUtil]
	mov rsi, 0
	lea rdi, [str_libSceSysUtil]
	call sys_dynlib_load_prx
	test rax, rax
	je resolve

resolve:
	; resolve sceSysUtilSendSystemNotificationWithText
	lea rdx, [asceSysUtilSendSystemNotificationWithText]
	lea rsi, [str_sceSysUtilSendSystemNotificationWithText]
	mov rdi, qword [libSceSysUtil]
	call sys_dynlib_dlsym

	; get libkernel handle
	mov rcx, 0
	lea rdx, [libkernel]
	mov rsi, 0
	lea rdi, [str_libkernel]
	call sys_dynlib_load_prx
	test rax, rax
	je resolve2

	mov rcx, 0
	lea rdx, [libkernel]
	mov rsi, 0
	lea rdi, [str_libkernelweb]
	call sys_dynlib_load_prx
	test rax, rax
	je resolve2

	mov rcx, 0
	lea rdx, [libkernel]
	mov rsi, 0
	lea rdi, [str_libkernelsys]
	call sys_dynlib_load_prx

resolve2:
	; resolve sceKernelUsleep
	lea rdx, [sceKernelUsleep]
	lea rsi, [str_sceKernelSleep]
	mov rdi, qword [libkernel]
	call sys_dynlib_dlsym

Loop:
	cmp byte[CommandIndex], 0
	jne FirstCall

	mov rdi, 100000
	mov r12, qword [sceKernelUsleep]
	call r12

	jmp Loop

FirstCall:
	cmp byte[CommandIndex], 1
	jne SecondCall

	mov byte [ShellCodeComplete], 0

	; Call sceKernelLoadStartModule
    mov r9, [pRes]
    mov r8, [pOpt]
    mov rcx, [flags]
    mov rdx, [argv]
    mov rsi, [argc]
    lea rdi, [ModulePath]
    mov r12, qword [asceKernelLoadStartModule]
	call r12

    mov qword [ModuleHandle], rax

    mov byte [ShellCodeComplete], 1

	mov byte[CommandIndex], 0

	mov rdi, 100000
	mov r12, qword [sceKernelUsleep]
	call r12

	jmp Loop

SecondCall:
	cmp byte[CommandIndex], 2
	jne ThirdCall

	mov byte [ShellCodeComplete], 0

	; Call sceKernelStopUnloadModule
    mov r9, qword [pRes]
    mov r8, qword [pOpt]
    mov rcx, [flags]
    mov rdx, qword [argp]
    mov rsi, qword [args]
    mov rdi, [handle]
    mov r12, qword [asceKernelStopUnloadModule]
	call r12

    mov qword [Result], rax

    mov byte [ShellCodeComplete], 1

	mov byte[CommandIndex], 0

	mov rdi, 100000
	mov r12, qword [sceKernelUsleep]
	call r12

	jmp Loop

ThirdCall:
	cmp byte[CommandIndex], 3
	jne FourthCall

	mov byte [ShellCodeComplete], 0

	; Call sceKernelLoadStartModule
    lea rsi, [message]
    mov rdi, [messageType]
    mov r12, qword [asceSysUtilSendSystemNotificationWithText]
	call r12

    mov qword [Result], rax

    mov byte [ShellCodeComplete], 1

	mov byte[CommandIndex], 0

	mov rdi, 100000
	mov r12, qword [sceKernelUsleep]
	call r12

	jmp Loop

FourthCall:
	cmp byte[CommandIndex], 4
	jne FithCall


	mov byte[CommandIndex], 0

	mov rdi, 100000
	mov r12, qword [sceKernelUsleep]
	call r12

	jmp Loop

FithCall:
	cmp byte[CommandIndex], 5
	jne EndLoop

	mov byte[CommandIndex], 0

	mov rdi, 0
	call sys_thr_exit
	retn

EndLoop:
	mov byte[CommandIndex], 0
	jmp Loop


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