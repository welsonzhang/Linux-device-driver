EXTRA_CFLAGS=-g
obj-m := scull.o

  KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/3.10.0-327.3.1.el7.x86_64/build/ M=$(PWD) modules
clean: 
	make -C /lib/modules/3.10.0-327.3.1.el7.x86_64/build/ M=$(PWD) clean

