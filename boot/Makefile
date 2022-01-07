$(BIN_DIR)/boot.bin:
	nasm -g -f bin boot.asm -o $(BIN_DIR)/boot.bin
	dd if=message.txt >> $(BIN_DIR)/boot.bin
	dd if=/dev/zero bs=512 count=1 >> $(BIN_DIR)/boot.bin # write null bytes to next disk sector or 512 bytes
