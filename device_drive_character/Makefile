obj-m+=ddc_1.o
obj-m+=ddc_2.o
obj-m+=ddc_3.o
obj-m+=ddc_4.o
obj-m+=ddc_5.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
