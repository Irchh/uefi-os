bits 64

; contains runnable code
section .text

global _start
global __chkstk

;
extern kernel_main
extern init_efi

__chkstk:
    ret

; arguments
; sysv: rdi, rsi, rdx, rcx, r8, r9, stack
; ms:   rcx, rdx, r8,  r9, stack
; always return in rax

; Move arguments to be System V ABI compatible
_start:
	mov rdi, rcx
	mov rsi, rsp
	mov rdx, rbp
    call init_efi
    call kernel_main
    jmp $

; required by uefi for some reason
section .reloc