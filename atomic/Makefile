
obj-m    := atomic.o

    KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) modules
	gcc test_atomic.c -o test_atomic
clean: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) clean

