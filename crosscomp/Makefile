obj-m += hello.o

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(ARM_KERNEL) M=$(PWD) modules

clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(ARM_KERNEL) M=$(PWD) clean
