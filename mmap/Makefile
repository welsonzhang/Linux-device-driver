
obj-m    := mmap_shared.o

    KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) modules
	gcc user_mmap.c -o user_mmap
clean: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) clean

