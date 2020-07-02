bits 64
section .text

global load_gdt
global read_tr

read_tr:
	xor rax, rax
	str rax
	ret

load_gdt:
	lgdt [rcx]
	mov ax, 0x40
	ltr ax
	mov ax, 0x08
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	pop rdi
	mov rax, 0x38
	push rax
	push rdi
	db 0x48
	retf