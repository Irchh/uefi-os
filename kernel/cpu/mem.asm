global read_cr3
global load_pml4t

read_cr3:
	mov rax, cr3
	ret

load_pml4t:
	cli
	;ret
	push rdi
	mov rdi, cr4
	or rdi, 1<<5
	mov cr4, rdi
	pop rdi
	mov cr3, rdi
	sti
	ret