BITS 64
DEFAULT REL

magic: db 'SHEL'
entry: dq shellcode

thr_initial: dq 0
SceSysCoreBase: dq 0
spawnProcess: dq 0
Result: dd 0
Complete: dd 0
param3: dq 0
param4: dq 0

; Data
ProcessID: dd -1
Location: db '/system/sys/decid.elf'
TitleID: db 'ORBIS1337'

shellcode:
    ; load thread into fs
	mov rdi, qword [thr_initial]
	mov rsi, qword [rdi]
	mov rdi, qword [rsi + 0x1E0]
	call amd64_set_fsbase

	; int umd::Manager::spawnProcess(SceKernelPid *, const char *, char *const *, const SceUmdFilter *, unsigned int, const char *)
	; Calling the load decid.elf
	;lea rdi, [ProcessID]
	;lea rsi, [Location] ; "/mnt/usb0/OrbisLib.elf"
	;mov r12, param3
	;lea rdx, [param3]
	;mov r12, param4
	;lea rcx, [param4]
	;mov r8d, 5
	;lea r9, [TitleID]
	;xor r9d, r9d
	mov r12, [spawnProcess]
	call r12
	mov [Result], rax
	mov dword [Complete], 1    

LoopExit:
	mov rdi, 0
	call sys_thr_exit
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