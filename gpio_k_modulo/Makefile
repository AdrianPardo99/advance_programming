obj-m += gpio_module.o
obj-m += gpio_module_cb.o
obj-m += gpio_module_ddc.o

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(ARM_KERNEL) M=$(PWD) modules

clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(ARM_KERNEL) M=$(PWD) clean
