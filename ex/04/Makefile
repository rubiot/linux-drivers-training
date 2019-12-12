KDIR := /opt/labs/ex/01/linux
ROOTFS := /opt/labs/ex/03/rootfs

obj-m += labmodule.o

module:
	@$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	@sudo $(MAKE) -C $(KDIR) M=$(PWD) INSTALL_MOD_PATH=$(ROOTFS) modules_install

clean:
	@$(MAKE) -C $(KDIR) M=$(PWD) clean
