#!/bin/bash
gdb-multiarch -ex "file build/kernel8.elf" -ex "target remote localhost:1234"
