# Makefile for test module

# If KERNELRELEASE is defined, we've been invoked from the

# kernel build system and can use its language.


# Otherwise we were called directly from the command

# line; invoke the kernel build system.


KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
obj-m := test_reg.o
  
default:
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules


clean:
	$(shell rm -f *.bak)
	$(shell rm -f test_reg_syscall.o)
	$(shell rm -f test_reg_syscall.ko)
	$(shell rm -f test_reg_syscall.mod.c)
	$(shell rm -f test_reg_syscall.mod.o)
