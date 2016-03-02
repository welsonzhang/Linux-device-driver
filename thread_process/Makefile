
obj-m    :=process_print.o process_thread_print.o 

    KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/3.10.0-327.3.1.el7.x86_64/build/ M=$(PWD) modules
	gcc thread.c -lpthread -o thread
clean: 
	make -C /lib/modules/3.10.0-327.3.1.el7.x86_64/build/ M=$(PWD) clean
	rm -rf thread
