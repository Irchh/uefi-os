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

_start:
    call init_efi
    call kernel_main
    ret

; required by uefi for some reason
section .reloc