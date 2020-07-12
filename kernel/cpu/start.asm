bits 64

; contains runnable code
section .text

global _start
global __chkstk

;
extern kernel_main
extern init_efi

loopForever:
    jmp loopForever

__chkstk:
    ret

; arguments
; sysv: rdi, rsi, rdx, rcx, r8, r9, stack
; ms:        rcx, rdx, r8,  r9, stack
; always return in rax

; Move arguments to be System V ABI compatible
_start:
	push rdi
	push rsi
	mov rdi, rcx
	mov rsi, rdx
    call init_efi
    call kernel_main
    pop rsi
    pop rdi
    ret

; required by uefi for some reason
section .reloc