FILES = $(BUILD_DIR)/$(ARCH_DIR)/kernel.asm.o

$(BIN_DIR)/$(ARCH_DIR)/kernel.bin: $(FILES)
	$(HOST)-ld -g -relocatable $(FILES) -o $(BUILD_DIR)/$(ARCH_DIR)/kernelfull.o
	$(HOST)-gcc -T ./$(ARCH_DIR)/linker.ld -o $(BIN_DIR)/$(ARCH_DIR)/kernel.bin -ffreestanding -O0 -nostdlib $(BUILD_DIR)/$(ARCH_DIR)/kernelfull.o

$(BUILD_DIR)/$(ARCH_DIR)/kernel.asm.o: ./$(ARCH_DIR)/kernel.asm
	nasm -g -f elf ./$(ARCH_DIR)/kernel.asm -o $(BUILD_DIR)/$(ARCH_DIR)/kernel.asm.o
