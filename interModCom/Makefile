ifeq ($(KERNELRELEASE), )
	KERNELDIR ?= /lib/modules/2.6.38.8/build
	PWD := $(shell pwd)
	PRINT_INC =$(PWD)/../include
	EXTRA_CFLAGS += -I $(PRINT_INC)
modules:
	$(MAKE) $(EXTRA_CFLAGS) -C $(KERNELDIR) M=$(PWD) modules
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
clean:
	rm -rf *.o *.ko *.mod.* *.order *.symvers
.PHONY: modules modules_install clean
else
	obj-m := add_sub.o
endif


--KERNELDIR := /lib/modules/2.6.35.6-45.fc14.i686/build
