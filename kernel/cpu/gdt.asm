bits 64
section .text

global load_gdt
global read_tr
global read_rsp

read_tr:
	xor rax, rax
	str rax
	ret

read_rsp:
	mov rax, rsp
	ret

load_gdt:
	lgdt [rdi]
	mov ax, 0x43
	ltr ax
	mov ax, 0x08
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	pop rdi
	mov rax, 0x10
	push rax
	push rdi
	db 0x48
	retf