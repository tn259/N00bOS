include $(ARCH_DIR)/make.config
include $(ARCH_DIR)/paging/make.config
include ./mm/heap/make.config
include ./drivers/ata/make.config
include ./libc/make.config # For now just link libc straight into the kernel binary

FILES = $(BUILD_DIR)/$(ARCH_DIR)/kernel.asm.o $(BUILD_DIR)/$(ARCH_DIR)/kernel.cpp.o $(KERNEL_ARCH_OBJS) $(KERNEL_ARCH_PAGING_OBJS) $(MEMORY_ARCH_OBJS) $(HEAP_ARCH_OBJS) $(ATA_ARCH_OBJS) $(LIBC_ARCH_OBJS)
INCLUDES = -I./ -I./$(ARCH_DIR)/ -I./$(ARCH_DIR)/paging -I./mm/ -I./mm/heap/ -I./libc/

$(BIN_DIR)/$(ARCH_DIR)/kernel.bin: $(FILES)
	$(HOST)-ld -g -relocatable $(FILES) -o $(BUILD_DIR)/$(ARCH_DIR)/kernelfull.o
	$(HOST)-gcc -T ./$(ARCH_DIR)/linker.ld -o $(BIN_DIR)/$(ARCH_DIR)/kernel.bin -ffreestanding -O0 -nostdlib $(BUILD_DIR)/$(ARCH_DIR)/kernelfull.o

# Assembly
$(BUILD_DIR)/$(ARCH_DIR)/%.asm.o: ./$(ARCH_DIR)/%.asm
	# TODO needs to be made conditional
	nasm -g -f elf $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.asm.o: ./$(ARCH_DIR)/paging/%.asm
	# TODO needs to be made conditional
	nasm -g -f elf $< -o $@

# The kernel main itself
$(BUILD_DIR)/$(ARCH_DIR)/kernel.cpp.o: ./kernel.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@

# all other cpp
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./$(ARCH_DIR)/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./$(ARCH_DIR)/paging/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./mm/heap/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./drivers/ata/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./libc/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@