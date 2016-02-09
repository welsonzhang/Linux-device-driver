CONFIG_MODULE_SIG=n

obj-m    := procdemo.o

    KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) modules
clean: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) clean

