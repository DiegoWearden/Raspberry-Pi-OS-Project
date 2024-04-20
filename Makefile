ARMGNU ?= aarch64-linux-gnu

# Regular compiler options
COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASMOPS = -Iinclude 

# Additional debug compiler options
DEBUG_COPS = $(COPS) -g -O0

BUILD_DIR = build
SRC_DIR = src

all : kernel8.img

debug: clean kernel8.img debug_qemu

clean :
	rm -rf $(BUILD_DIR) *.img 

# Command to execute debug_qemu.sh
debug_qemu:
	./debug_qemu.sh

# Rule for C++ source files with debug options
$(BUILD_DIR)/%_cc.o: $(SRC_DIR)/%.cc
	mkdir -p $(@D)
	$(ARMGNU)-g++ $(DEBUG_COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

CC_FILES = $(wildcard $(SRC_DIR)/*.cc)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(CC_FILES:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%_cc.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
