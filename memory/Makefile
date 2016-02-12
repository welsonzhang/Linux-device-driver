
obj-m    := slab_cache.o mempool_cache.o

    KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) modules
clean: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) clean

