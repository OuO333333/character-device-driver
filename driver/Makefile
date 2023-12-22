PWD := $(shell pwd) 
KVERSION := $(shell uname -r)
KERNEL_DIR = /usr/src/linux-headers-$(KVERSION)/
C_FLAGS     := -Wall
MODULE_NAME = mychardev

CC := x86_64-linux-gnu-gcc-11

ccflags-y += $(C_FLAGS)
obj-m := $(MODULE_NAME).o

 
all: 
	make -C $(KERNEL_DIR) M=$(PWD) modules CC=$(CC)
clean: 
	make -C $(KERNEL_DIR) M=$(PWD) clean CC=$(CC)