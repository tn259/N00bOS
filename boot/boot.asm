; Useful instruction set reference
; http://www.gabrielececchetti.it/Teaching/CalcolatoriElettronici/Docs/i8086_instruction_set.pdf

; Interrupts reference
; http://www.ctyme.com/intr/int.htm

ORG 0 ; BIOS looks for something to run at this address
BITS 16 ; Running in 16 bit Real mode

jmp 0x7c0:start ; code segment starts at 0x7c0, offset is at the start label to start executing

start:

._step1:
    ; first explicitly set the segment registers as there is no guarantee as to what they will be set to
    cli ; disable hw interrupts via interrupt flag
    mov ax, 0x7c0 ; cannot mov directly into the segment registers
    mov ds, ax ; set to 0x7c0
    mov es, ax ; set to 0x7c0
    mov ax, 0x00
    mov ss, ax ; set to 0x00
    mov sp, 0x7c00
    sti ; enable hw interrupts via interrupt flag

._step2:
; http://www.ctyme.com/intr/rb-0607.htm

; AH = 02h
; AL = number of sectors to read (must be nonzero)
; CH = low eight bits of cylinder number
; CL = sector number 1-63 (bits 0-5)
; high two bits of cylinder (bits 6-7, hard disk only)
; DH = head number
; DL = drive number (bit 7 set for hard disk)
; ES:BX -> data buffer

; Return:
; CF set on error
; if AH = 11h (corrected ECC error), AL = burst length
; CF clear if successful
; AH = status (see #00234)
; AL = number of sectors transferred (only valid if CF set for some BIOSes)

    mov ah, 2
    mov al, 1
    mov ch, 0 ; low 8 bits
    mov cl, 2 ; read sector 2 (start at 1 for CHS where 1 is our bootloader sector)
    mov dh, 0 ; first head ? don't need to set dl because it is already set for us from step 1?
    mov bx, buffer ; es is already set to 0x7c0
    int 0x13 ; issue interrupt
    jc ._error ; jump on carry i.e. failure
    mov si, buffer
    call print
    jmp $ ; unconditional jump to the same line i.e. loop forever

._error:
    mov si, error_message
    call print
    jmp $

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

error_message: db 'Failed to load sector', 0

times 510-($ - $$) db 0 ; for up t0 510 bytes pad the rest of the bootloader with 0
dw 0xAA55 ; bootloader signature 510 bytes into the bootloader 55AA bytswapped

buffer: ; label for sector after bootloader signature 
