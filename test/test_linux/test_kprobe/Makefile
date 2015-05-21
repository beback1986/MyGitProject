ifneq ($(KERNELRELEASE),)
	obj-m = dump_stack.o
else
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -rf *.o *.ko *.mod.c .tmp_versions .*.cmd .depend

