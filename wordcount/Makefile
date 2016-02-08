CONFIG_MODULE_SIG=n

obj-m    := wordcount.o

    KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/3.10.0-327.3.1.el7.x86_64/build/ M=$(PWD) modules
	gcc wordcount_test.c -o wordcount_test
clean: 
	make -C /lib/modules/3.10.0-327.3.1.el7.x86_64/build/ M=$(PWD) clean

