.section .text

.global load_gdt

load_gdt:
	lgdt [rdi]
	mov ax, 0x40
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
	rex64 retf