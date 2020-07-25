bits 64
section .text
align 4

global load_idt
global div_by_0
global syscall_test
global goto_user_space
extern PrintChar

load_idt:
	lidt [rdi]
	sti
	ret

div_by_0:
	mov ax, 9
	mov bl, 0
	div bl
	ret

syscall_test:
	push rax
	push rdi
	push rsi
	push rdx
	mov rax, 1
	mov rdi, 0
	mov rsi, HELLO
	mov rdx, 14
	syscall
	pop rdx
	pop rsi
	pop rdi
	pop rax
	ret

goto_user_space:
	push qword 0x23	; ss
	push qword rsp	; rsp
	push qword 0x202; rflags
	push qword 0x2B	; cs
	push qword TST	; rip
	iret
TST:
	jmp TST
	;mov rdi, 'a'
	;call PrintChar

HELLO:
	db "Hello, World!", 0x0A