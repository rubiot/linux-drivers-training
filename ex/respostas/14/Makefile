KDIR   := /opt/labs/ex/01/linux
ROOTFS := /opt/labs/ex/03/rootfs

ccflags-y := -g -O0

obj-m += pwm-imx.o

module:
	@$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	@sudo $(MAKE) -C $(KDIR) M=$(PWD) INSTALL_MOD_PATH=$(ROOTFS) modules_install

clean:
	@$(MAKE) -C $(KDIR) M=$(PWD) clean
