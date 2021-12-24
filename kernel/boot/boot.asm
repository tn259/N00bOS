; Useful instruction set reference
; http://www.gabrielececchetti.it/Teaching/CalcolatoriElettronici/Docs/i8086_instruction_set.pdf

; Interrupts reference
; http://www.ctyme.com/intr/int.htm

ORG 0x7c00 ; BIOS looks for something to run at this address
BITS 16 ; Running in 16 bit Real mode

start:
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
