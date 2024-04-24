qemu-system-aarch64 \
    -M raspi3b \
    -cpu cortex-a53 \
    -kernel build/kernel8.elf \
    -append "root=/dev/mmcblk0p2 rw" \
    -m 1G \
    -smp 4 \
    -serial stdio \
    -s -S
    -append "console=ttyAMA0,115200"
