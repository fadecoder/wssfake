dmesg -c
insmod run_pmu.ko
taskset -c 0 /wssfake/wss_fake 1000 1
rmmod run_pmu.ko
dmesg 
