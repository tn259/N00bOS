[BITS 32] ; https://stackoverflow.com/questions/31989439/nasm-square-brackets-around-directives-like-bits-16 - explanation on square brackets here
global _start ; exports the _start symbol so can be used by the linker
CODE_SEGMENT equ 0x08
DATA_SEGMENT equ 0x10

_start:
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x00200000
    mov esp, ebp ; stack and base pointer can now be set further in memory

    ; Enable the A20 line
    ; See https://www.win.tue.nl/~aeb/linux/kbd/A20.html
    ; https://wiki.osdev.org/A20_Line
    ; Just an annoying accident of history makes this necessary
    in al, 0x92
    or al, 2
    out 0x92, al

    jmp $

