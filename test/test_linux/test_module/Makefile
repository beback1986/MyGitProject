# Makefile for test module

# If KERNELRELEASE is defined, we've been invoked from the

# kernel build system and can use its language.


# Otherwise we were called directly from the command

# line; invoke the kernel build system.


KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
obj-m := test.o
  
default:
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules


clean:
	$(shell rm -f *.bak)
	$(shell rm -f test.o)
	$(shell rm -f test.ko)
	$(shell rm -f test.mod.c)
	$(shell rm -f test.mod.o)
	$(shell rm -f *~)
	$(shell rm -f .test*)
	$(shell rm -rf .tmp_versions)
	$(shell rm -rf Module.markers)
	$(shell rm -rf modules.order)
	$(shell rm -rf Module.symvers)
