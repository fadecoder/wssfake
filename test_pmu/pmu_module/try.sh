array=$1
round=$2
dmesg -c
insmod run_pmu.ko
taskset -c 0 /wssfake/wss_fake $array $round
rmmod run_pmu.ko
dmesg 
