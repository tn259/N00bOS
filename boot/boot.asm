; Useful instruction set reference
; http://www.gabrielececchetti.it/Teaching/CalcolatoriElettronici/Docs/i8086_instruction_set.pdf

; Interrupts reference
; http://www.ctyme.com/intr/int.htm

ORG 0 ; BIOS looks for something to run at this address
BITS 16 ; Running in 16 bit Real mode

jmp 0x7c0:start ; code segment starts at 0x7c0, offset is at the start label to start executing

start:
    ; first explicitly set the segment registers as there is no guarantee as to what they will be set to
    cli ; disable hw interrupts via interrupt flag
    mov ax, 0x7c0 ; cannot mov directly into the segment registers
    mov ds, ax ; set to 0x7c0
    mov es, ax ; set to 0x7c0
    mov ax, 0x00
    mov ss, ax ; set to 0x00
    mov sp, 0x7c00
    sti ; enable hw interrupts via interrupt flag
    ; second do the rest
    mov si, message
    call print
    jmp $ ; unconditional jump to the same line i.e. loop forever

print:
    mov bx, 0
.loop: ; sub-label within print
    lodsb ; load byte to al from si then increment pointer to next byte in si
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0eh ; http://www.ctyme.com/intr/rb-0106.htm
    int 0x10
    ret

message: db 'Hello World!', 0

times 510-($ - $$) db 0 ; for up t0 510 bytes pad the rest of the bootloader with 0
dw 0xAA55 ; bootloader signature 510 bytes into the bootloader 55AA bytswapped 
