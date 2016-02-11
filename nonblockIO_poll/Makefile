CONFIG_MODULE_SIG=n

obj-m    := poll_select.o

    KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) modules
	gcc test_select.c -o test_select
clean: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) clean

