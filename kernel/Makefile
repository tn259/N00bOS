FILES = $(BUILD_DIR)/kernel.asm.o 

$(BIN_DIR)/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o $(BUILD_DIR)/kernelfull.o
	i686-elf-gcc -T ./linker.ld -o $(BIN_DIR)/kernel.bin -ffreestanding -O0 -nostdlib $(BUILD_DIR)/kernelfull.o

$(BUILD_DIR)/kernel.asm.o: ./kernel.asm
	nasm -g -f elf ./kernel.asm -o $(BUILD_DIR)/kernel.asm.o
