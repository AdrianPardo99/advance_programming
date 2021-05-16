#!/usr/bin/env bash
# Raspberry Pi 3, 3B, 3B+, 2, MC3
cd ~/linux
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
make -j6 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
