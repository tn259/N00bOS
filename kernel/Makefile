include $(ARCH_DIR)/make.config
include $(ARCH_DIR)/paging/make.config
include $(ARCH_DIR)/idt/make.config
include $(ARCH_DIR)/io/make.config
include ./mm/heap/make.config
include ./disk/make.config
include ./drivers/ata/make.config
include ./fs/make.config
include ./libc/make.config # For now just link libc straight into the kernel binary

FILES = $(BUILD_DIR)/$(ARCH_DIR)/kernel.asm.o $(BUILD_DIR)/$(ARCH_DIR)/kernel.cpp.o $(KERNEL_ARCH_OBJS) $(KERNEL_ARCH_PAGING_OBJS) $(IDT_ARCH_OBJS) $(IO_ARCH_OBJS) $(MEMORY_ARCH_OBJS) $(HEAP_ARCH_OBJS) $(ATA_ARCH_OBJS) $(DISK_ARCH_OBJS) $(FS_ARCH_OBJS) $(LIBC_ARCH_OBJS)
INCLUDES = -I./ -I./$(ARCH_DIR)/ -I./$(ARCH_DIR)/paging -I./$(ARCH_DIR)/idt -I./$(ARCH_DIR)/io -I./mm/ -I./mm/heap/ -I./disk -I./drivers/ata -I./fs -I./libc/

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
$(BUILD_DIR)/$(ARCH_DIR)/%.asm.o: ./$(ARCH_DIR)/idt/%.asm
	# TODO needs to be made conditional
	nasm -g -f elf $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.asm.o: ./$(ARCH_DIR)/io/%.asm
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
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./$(ARCH_DIR)/idt/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./$(ARCH_DIR)/io/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./mm/heap/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./disk/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./drivers/ata/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./fs/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
$(BUILD_DIR)/$(ARCH_DIR)/%.cpp.o: ./libc/%.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) $(CXXFLAGS) -MMD -c $< -o $@
