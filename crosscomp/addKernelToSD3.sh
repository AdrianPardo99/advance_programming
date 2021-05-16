#!/usr/bin/env bash
# Raspberry Pi 3, 3B, 3B+, 2, MC3
BOOT="mmcblk0p1"
ROOTFS="mmcblk0p2"
KERNEL=kernel7
cd ~/linux
mkdir -p mnt/boot mnt/rootfs
sudo umount /dev/${BOOT}
sudo umount /dev/${ROOTFS}
sudo mount /dev/${BOOT} mnt/boot
sudo mount /dev/${ROOTFS} mnt/rootfs

sudo env PATH=$PATH make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=mnt/rootfs modules_install
sudo cp mnt/boot/$KERNEL.img mnt/boot/$KERNEL-backup.img
sudo cp arch/arm/boot/zImage mnt/boot/$KERNEL.img
sudo cp arch/arm/boot/dts/*.dtb mnt/boot/
sudo cp arch/arm/boot/dts/overlays/*.dtb* mnt/boot/overlays/
sudo cp arch/arm/boot/dts/overlays/README mnt/boot/overlays/
sudo umount mnt/boot
sudo umount mnt/rootfs
