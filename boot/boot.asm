; Useful instruction set reference
; http://www.gabrielececchetti.it/Teaching/CalcolatoriElettronici/Docs/i8086_instruction_set.pdf

; Interrupts reference
; http://www.ctyme.com/intr/int.htm

ORG 0x7c00 ; BIOS looks for something to run at this address
BITS 16 ; Running in 16 bit Real mode

; Make the GDT offsets constants
CODE_SEGMENT equ gdt_code - gdt_start
DATA_SEGMENT equ gdt_data - gdt_start

jmp 0:start ; starts at 0x7c0 (absolute address set with ORG), offset is at the start label to start executing

start:

.clear_segments:
    ; first explicitly set the segment registers as there is no guarantee as to what they will be set to
    cli ; disable hw interrupts via interrupt flag
    mov ax, 0x0 ; cannot mov directly into the segment registers
    mov ds, ax ; set to 0x7c0
    mov es, ax ; set to 0x7c0
    mov ss, ax ; set to 0x00
    mov sp, 0x7c00
    sti ; enable hw interrupts via interrupt flag

.load_protected:
    cli
    lgdt[gdt_descriptor]  ; load global descriptor table
    mov eax, cr0          ; Bit 0 or cr0 toggles protected mode
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEGMENT:load32 ; far jmp <Segment>:<Address within segment>

;**********************************************************************
; GDT - See http://www.brokenthorn.com/Resources/OSDev8.html for a good
;       explanation
; Just set default values so we can access all data 
;**********************************************************************
gdt_start:
gdt_null:
    dd 0x0 ; doubleword (4 bytes)
    dd 0x0
; CODE SEGMENT at offset 0x8
gdt_code: ; CS SHOULD POINT HERE
    dw 0xffff ; Segment limit - 0xffff allows us to address the full range
    dw 0      ; Base (low) 0-15
    db 0      ; Base (middle) 16-23
    db 0x9a   ; Access byte (executable)
    db 11001111b ; Granularity byte
    db 0      ; Base (high)
; DATA SEGMENT at offset 0x10
gdt_data: ; DS, SS, ES, FS, GS SHOULD POINT HERE 
    dw 0xffff ; Segment limit - 0xffff allows us to address the full range
    dw 0      ; Base (low) 0-15
    db 0      ; Base (middle) 16-23
    db 0x92   ; Access byte (read/write)
    db 11001111b ; Granularity byte
    db 0      ; Base (high)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Size
    dd gdt_start ; Offset

[BITS 32] ; https://stackoverflow.com/questions/31989439/nasm-square-brackets-around-directives-like-bits-16 - explanation on square brackets here
load32:
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x00200000
    mov esp, ebp ; stack and base pointer can now be set further in memory
    jmp $

;***********************************************************************
; BOOTLOADER SIGNATURE
;*********************************************************************** 
times 510-($ - $$) db 0 ; for up t0 510 bytes pad the rest of the bootloader with 0
dw 0xAA55 ; bootloader signature 510 bytes into the bootloader 55AA bytswapped
