# Makefile for mding module

# If KERNELRELEASE is defined, we've been invoked from the

# kernel build system and can use its language.


# Otherwise we were called directly from the command

# line; invoke the kernel build system.


KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
obj-m := mding.o
  
default:
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules


clean:
	$(shell rm -f *.bak)
	$(shell rm -f mding.o)
	$(shell rm -f mding.ko)
	$(shell rm -f mding.mod.c)
	$(shell rm -f mding.mod.o)
	$(shell rm -rf .mding.* Module.symvers modules.order .tmp_versions/ Module.symvers)
