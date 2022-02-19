section .asm

global insb
global insw
global outb
global outw


; https://c9x.me/x86/html/file_module_x86_id_139.html
insb:
    push ebp
    mov ebp, esp

    xor eax, eax    ; zero out the eax reg
    mov edx, [ebp+8] ; first argument into edx reg
    in al, dx

    leave
    ret ; eax is always the return value
insw:
    push ebp
    mov ebp, esp

    xor eax, eax    ; zero out the eax reg
    mov edx, [ebp+8] ; first argument into edx reg
    in ax, dx

    leave
    ret ; eax is always the return value

; https://c9x.me/x86/html/file_module_x86_id_222.html
outb:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12] ; second argument into edx reg
    mov edx, [ebp+8] ; first argument into edx reg
    out dx, al

    leave
    ret ; eax is always the return value
outw:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12] ; second argument into edx reg
    mov edx, [ebp+8] ; first argument into edx reg
    out dx, ax

    leave
    ret ; eax is always the return value