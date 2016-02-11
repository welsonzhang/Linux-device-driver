CONFIG_MODULE_SIG=n

obj-m    := signal.o aio_signal.o

    KVERSION = $(shell uname -r)
all: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) modules
	gcc test_rw_signal.c -o test_rw_signal
	gcc test_rw_aio_signal.c -o test_rw_aio_signal -lrt
	gcc test_aio_callback.c -o test_aio_callback -lrt
	gcc test_signal.c -o test_signal
	gcc test_io_signal.c -o test_io_signal
	gcc test_aio_rw.c -o test_aio_rw -lrt
clean: 
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) clean

