BITS 64
DEFAULT REL

magic: db 'SHEL'
entry: dq shellcode

thr_initial: dq 0
ShellCodeComplete: db 0
CommandIndex: db 0
ShouldExit: db 0

Messagetype: dd 0
Message: db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

;Addresses
sceKernelUsleep: dq 0
sceSysUtilSendNotificationRequest: dq 0
sceSysUtilSendSystemNotificationWithText: dq 0
libkernel: dq 0
libSceSysUtil: dq 0
str_libSceSysUtil: db 'libSceSysUtil.sprx', 0
str_libkernel: db 'libkernel.sprx', 0
str_libkernelweb: db 'libkernel_web.sprx', 0
str_libkernelsys: db 'libkernel_sys.sprx', 0
str_sceKernelSleep: db 'sceKernelUsleep', 0
str_sceSysUtilSendNotificationRequest: db 'sceSysUtilSendNotificationRequest', 0
str_sceSysUtilSendSystemNotificationWithText: db 'sceSysUtilSendSystemNotificationWithText', 0

shellcode:
    ; load thread into fs
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
    test rax, rax
	jne LoopExit

    mov rcx, 0
	lea rdx, [libSceSysUtil]
	mov rsi, 0
	lea rdi, [str_libSceSysUtil]
	call sys_dynlib_load_prx
    test rax, rax
	jne LoopExit

resolve:
    ; resolve sceKernelUsleep
	lea rdx, [sceKernelUsleep]
	lea rsi, [str_sceKernelSleep]
	mov rdi, qword [libkernel]
	call sys_dynlib_dlsym

    ; resolve sceSysUtilSendNotificationRequest
	lea rdx, [sceSysUtilSendNotificationRequest]
	lea rsi, [str_sceSysUtilSendNotificationRequest]
	mov rdi, qword [libSceSysUtil]
	call sys_dynlib_dlsym

    ; resolve sceSysUtilSendSystemNotificationWithText
	lea rdx, [sceSysUtilSendSystemNotificationWithText]
	lea rsi, [str_sceSysUtilSendSystemNotificationWithText]
	mov rdi, qword [libSceSysUtil]
	call sys_dynlib_dlsym

LoopStart:
	cmp byte[ShouldExit], 1
	je LoopExit

FirstCall:
	cmp byte[CommandIndex], 1
	jne SecondCall

    mov rsi, 1
    lea rdi, [Message]
    mov r12, qword [sceSysUtilSendNotificationRequest]
	call r12
    mov byte [ShellCodeComplete], 1
	mov byte[CommandIndex], 0

	jmp EndofCase
SecondCall:
	cmp byte[CommandIndex], 2
	jne EndofCase
	
    lea rsi, [Message]
    mov rdi, [Messagetype]
    mov r12, qword [sceSysUtilSendSystemNotificationWithText]
	call r12
    mov byte [ShellCodeComplete], 1
	mov byte[CommandIndex], 0

EndofCase:
	mov rdi, 100000
	mov r12, qword [sceKernelUsleep]
	call r12
	jmp LoopStart        

LoopExit:
	mov rdi, 0
	call sys_thr_exit
	retn



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