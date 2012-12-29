ifeq ($(KERNELRELEASE), )
	KERNELDIR ?= /lib/modules/2.6.38.8/build
	PWD := $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
clean:
	rm -rf *.o *.ko *.mod.* *.order *.symvers
else
	obj-m := HelloWorld.o
endif


--KERNELDIR := /lib/modules/2.6.35.6-45.fc14.i686/build


