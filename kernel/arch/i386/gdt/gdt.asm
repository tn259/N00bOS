section .asm
global gdt_load

gdt_load:
    mov eax, [esp+4]             ; Load the start address
    mov [gdt_descriptor+2], eax
    mov eax, [esp+8]             ; Load the size
    mov [gdt_descriptor], ax
    lgdt [gdt_descriptor]        ; Load the data into gdt register
    ret

section .data
gdt_descriptor:
    dw 0x00 ; Size
    dd 0x00 ; GDT Start address