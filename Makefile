RPI_VERSION ?= 3

ARMGNU ?= aarch64-linux-gnu

# Update COPS for C++ by adding C++ specific flags if necessary
CXXOPS = -DRPI_VERSION=$(RPI_VERSION) -Wall -nostdlib -nostartfiles -ffreestanding \
         -Iinclude -mgeneral-regs-only -mno-outline-atomics -fpermissive -fno-rtti -fno-exceptions

DEBUG_COPS = $(CXXOPS) -g -O0

ASMOPS = -Iinclude

BUILD_DIR = build
SRC_DIR = src

all : kernel8.img

debug: clean kernel8.img debug_qemu

debug_qemu:
	./debug_qemu.sh

clean :
	rm -rf $(BUILD_DIR) *.img 

# Rule for C source files
$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(CXXOPS) -MMD -c $< -o $@

# Rule for C++ source files
$(BUILD_DIR)/%_cc.o: $(SRC_DIR)/%.cc
	mkdir -p $(@D)
	$(ARMGNU)-g++ $(DEBUG_COPS) $(CXXOPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
CPP_FILES = $(wildcard $(SRC_DIR)/*.cc)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(CPP_FILES:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%_cc.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	@echo "Building for RPI $(value RPI_VERSION)"
	@echo ""
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
	sync


armstub/build/armstub_s.o: armstub/src/armstub.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(CXXOPS) -MMD -c $< -o $@

armstub: armstub/build/armstub_s.o
	$(ARMGNU)-ld --section-start=.text=0 -o armstub/build/armstub.elf armstub/build/armstub_s.o
	$(ARMGNU)-objcopy armstub/build/armstub.elf -O binary armstub-new.bin
	cp armstub-new.bin $(BOOTMNT)/
	sync