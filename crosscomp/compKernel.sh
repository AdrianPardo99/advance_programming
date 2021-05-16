#!/usr/bin/env bash
# Raspberry Pi Zero, Pi 1, MC
cd ~/linux
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcmrpi_defconfig
make -j6 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
