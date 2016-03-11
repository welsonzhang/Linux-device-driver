
obj-m    := local_time.o timer_demo.o

    KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) modules
	gcc user_local_time.c -o user_local_time
	gcc test_timer.c -o test_timer
clean: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) clean

