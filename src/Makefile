lcd16x2-objs := lcd.o lcd_main.o
obj-m := lcd16x2.o

KERNEL = /home/pskrgag/Documents/bbb_workspace/build-linux
COMPILER = arm-linux-gnueabihf-
ARCH = arm

all:
	$(MAKE) -C $(KERNEL) ARCH=$(ARCH) CROSS_COMPILE=$(COMPILER) M=$(shell pwd) modules
clean:
	$(MAKE) -C $(KERNEL) ARCH=$(ARCH) CROSS_COMPILE=$(COMPILER) M=$(shell pwd) clean
