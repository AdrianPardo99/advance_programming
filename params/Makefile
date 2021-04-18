obj-m+=param-1.o
obj-m+=param-2.o
obj-m+=param-3.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
