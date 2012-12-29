ifeq ($(KERNELRELEASE), )
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
clean:
	rm -rf *.o *.ko *.mod.* *.order *.symvers
else
	obj-m := second.o
endif


#--KERNELDIR := /lib/modules/2.6.35.6-45.fc14.i686/build


