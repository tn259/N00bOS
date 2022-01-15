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
    ;jmp CODE_SEGMENT:load32 ; far jmp <Segment>:<Address within segment>
    jmp $

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

[BITS 32]
; https://wiki.osdev.org/ATA_read/write_sectors
load32: ; No longer in realmode so we have to write a driver to load the kernel into memory
    mov eax, 1 ; sector to load from, 0 is the boot sector
    mov ecx, 100 ; how many sectors to load, 100 sectors
    mov edi, 0x0100000 ; the starting address to load to, 1MB
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax ; Back up the LBA
    ; Send the highest 8 bits of the lba to hard disk controller
    shr eax, 24
    or eax, 0xE0 ; Select the master drive
    mov dx, 0x1F6 ; Write the 8 bits to this port
    out dx, al
    ; Finished sending the highest 8 bits of the lba

    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; Finished sending the total sectors to read

    ; Send more bits of the lba
    mov eax, ebx ; backup the lba
    mov dx, 0x1F3
    out dx, al
    ; Finished sending more bits of the lba

    ; Send more bits of the lba
    mov dx, 0x1F4
    mov eax, ebx ; backup the lba
    shr eax, 8
    out dx, al
    ; Finished sending more bits of the lba

    ; Send upper 16 bits of the lba
    mov dx, 0x1F5
    mov eax, ebx ; backup the lba
    shr eax, 16
    out dx, al
    ; Finished sending upper 16 bits of the lba

    ; Will be explained better in C/C++ later on
    mov dx 0x1F7
    mov al, 0x20
    out dx, al 

; Read all sectors into memory
.next_sector:
    push ecx

; Checking if we need to re-read
.try_again
    mov dx, 0x1F7
    in al, dx
    test al, 8 ; is bit 8 set ?
    jz .try_again
    ; Read 256 words at a time i.e. 512 byts i.e 1 sector
    mov ecx, 256
    mov dx, 0x1F0
    rep insw # read a word from 0x1F0 port in dx and store in edi which is 0x0100000 (kernel)
    pop ecx
    loop .next_sector
    ; End of reading sectors into memory
    ret

;***********************************************************************
; BOOTLOADER SIGNATURE
;*********************************************************************** 
times 510-($ - $$) db 0 ; for up t0 510 bytes pad the rest of the bootloader with 0
dw 0xAA55 ; bootloader signature 510 bytes into the bootloader 55AA bytswapped
