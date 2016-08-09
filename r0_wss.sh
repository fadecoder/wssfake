#!/bin/bash
rm data.txt -f
xm vcpu-set 0 1
xm vcpu-pin 0 0 1
xm vcpu-pin 1 0 1
signal_all=`xenstore-read /local/domain/1/vmm/wss_signal_all`
while [[ "$signal_all" != "begin_all" ]]; do
	sleep 0.1
	echo $signal_all
	signal_all=`xenstore-read /local/domain/1/vmm/wss_signal_all`
	echo 2
done
./xc -xt
time=1
while [[ "$signal_all" != "finished_all" ]]; do
	./xc -r 1
	while [[ "$signal" != "begin" && "$signal_all" != "finished_all" ]]; do
		echo 6
		sleep 0.1
		signal_all=`xenstore-read /local/domain/1/vmm/wss_signal_all`
		signal=`xenstore-read /local/domain/1/vmm/wss_signal`
	done
	while [[ "$signal" != "finished" ]]; do
		sleep 0.1
		echo 3
		signal=`xenstore-read /local/domain/1/vmm/wss_signal`
	done
	echo 4
	wss_e=`./xc -e 1 | cut -d, -f 3`
	wss_r=`xenstore-read /local/domain/1/vmm/wss`
	((wss_e=wss_e/256))
	printf "%s %s %s\n" $time $wss_e $wss_r >> data.txt
	((time++))
	signal_all=`xenstore-read /local/domain/1/vmm/wss_signal_all`
done
echo 5
