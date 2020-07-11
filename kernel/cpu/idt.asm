bits 64
section .text
align 4

global load_idt

load_idt:
	lidt [rcx]
	ret