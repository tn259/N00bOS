include $(ARCH_DIR)/make.config

#CPP_FILES=$(shell find . -name "*.cpp")
FILES = $(BUILD_DIR)/$(ARCH_DIR)/kernel.asm.o $(BUILD_DIR)/$(ARCH_DIR)/kernel.cpp.o $(KERNEL_ARCH_OBJS)
INCLUDES = -I./ -I./$(ARCH_DIR)/

$(BIN_DIR)/$(ARCH_DIR)/kernel.bin: $(FILES)
	$(HOST)-ld -g -relocatable $(FILES) -o $(BUILD_DIR)/$(ARCH_DIR)/kernelfull.o
	$(HOST)-gcc -T ./$(ARCH_DIR)/linker.ld -o $(BIN_DIR)/$(ARCH_DIR)/kernel.bin -ffreestanding -O0 -nostdlib $(BUILD_DIR)/$(ARCH_DIR)/kernelfull.o

$(BUILD_DIR)/$(ARCH_DIR)/%.asm.o: ./$(ARCH_DIR)/%.asm
	# TODO needs to be made conditional
	nasm -g -f elf $< -o $@

$(BUILD_DIR)/$(ARCH_DIR)/kernel.cpp.o: ./kernel.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@

$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./$(ARCH_DIR)/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
