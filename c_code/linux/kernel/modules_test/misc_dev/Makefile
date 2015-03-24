obj-m := misc_dev.o
PWD := $(shell pwd)
default:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.cmd *.ko* *.mod.c .tmp* *.symvers *.order .*.cmd
