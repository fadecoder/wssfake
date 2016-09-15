obj-m := run_pmu.o
kernel_path=/common/mb/linux-3.10.12
all:
	make -C $(kernel_path) M=$(PWD) modules
clean:
	make -C $(kernel_path) M=$(PWD) clean
