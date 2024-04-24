#!/bin/bash

sudo qemu-system-aarch64 -M raspi3b -append "rw earlyprintk loglevel=8 console=ttyAMA0,115200 dwc_otg.lpm_enable=0 root=/dev/mmcblk0p2 rootdelay=1" -dtb bcm2710-rpi-3-b.dtb -sd 2020-08-20-raspios-buster-armhf-full.img -kernel kernel8.img -m 1G -smp 4 -serial stdio -usb -device usb-mouse -device usb-kbd
