[BITS 32] ; https://stackoverflow.com/questions/31989439/nasm-square-brackets-around-directives-like-bits-16 - explanation on square brackets here
global _start ; exports the _start symbol so can be used by the linker
global div_zero ; exports the _start symbol so can be used by the linker
extern kernel_main ; imports the C function
CODE_SEGMENT equ 0x08
DATA_SEGMENT equ 0x10
MASTER_PIC_COMMAND_PORT equ 0x20
MASTER_PIC_DATA_PORT equ 0x21

_start:
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x00200000
    mov esp, ebp ; stack and base pointer can now be set further in memory

    ; Remap master PIC
    mov al, 00010001b ; init mode
    out MASTER_PIC_COMMAND_PORT, al ; Tell master pic

    mov al, 0x20 ; Interrupt 0x20 is where master ISR should start
    out MASTER_PIC_DATA_PORT, al

    mov al, 00000001b ; Put the PIC in x86 mode
    out MASTER_PIC_DATA_PORT, al
    ; End of remap master PIC

    ; Enable the A20 line
    ; See https://www.win.tue.nl/~aeb/linux/kbd/A20.html
    ; https://wiki.osdev.org/A20_Line
    ; Just an annoying accident of history makes this necessary
    in al, 0x92
    or al, 2
    out 0x92, al

    call kernel_main

    jmp $

div_zero:
    push ebp
    mov ebp, esp
    int 0
    leave
    ret
    ;mov eax, 0
    ;div eax

; We have to pad for alignment with the C compiler as _start will be shared with the C code
; If we do not do this there may be a mismatch between addesses and their instructions
; so we could end up at instructions we did not intend to run
times 512-($ - $$) db 0 ; for up to 512 bytes pad the rest with 0
