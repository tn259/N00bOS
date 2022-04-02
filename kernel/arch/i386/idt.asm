section .asm

extern int21h_handler ; import from C
extern no_interrupt_handler ; import from C

global enable_interrupts
global disable_interrupts
global idt_load
global int21h ; export to C
global no_interrupt

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

idt_load:
    push ebp        ; save base pointer of callee frame
    mov ebp, esp    ; basepointer of this frame is now the stack frame

    mov ebx, [ebp+8] ; first argument value of this function; ebp+4 is the return address of function caller 
    lidt [ebx]       ; load interrupt descriptor table at this value

    pop ebp         ; retrieve old callee basepointer
    ret

int21h: ; Example keyboard IRQ
    cli
    pushad ; push/pop all general purpose registers
    call int21h_handler
    popad
    sti
    iret

no_interrupt: ; Example keyboard IRQ
    cli
    pushad ; push/pop all general purpose registers
    call no_interrupt_handler
    popad
    sti
    iret