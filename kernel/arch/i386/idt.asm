section .asm

global idt_load
idt_load:
    push ebp        ; save base pointer of callee frame
    mov ebp, esp    ; basepointer of this frame is now the stack frame

    mov ebx, [ebp+8] ; first argument value of this function; ebp+4 is the return address of function caller 
    lidt [ebx]       ; load interrupt descriptor table at this value

    pop ebp         ; retrieve old callee basepointer
    ret
