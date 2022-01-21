# Just make this unconditional on i386 for now as nasm only applies for this and x86_64
# TODO change this to be conditional once we introduce new architectures
$(BIN_DIR)/arch/i386/boot.bin: ./arch/i386/boot.asm
	nasm -g -f bin ./arch/i386/boot.asm -o $(BIN_DIR)/arch/i386/boot.bin
