section .asm

global tss_load

tss_load:
    push ebp
    mov ebp, esp
    mov ax, [ebp+8] ; TSS segment number
    ltr ax          ; Load task register https://c9x.me/x86/html/file_module_x86_id_163.html
    leave
    ret