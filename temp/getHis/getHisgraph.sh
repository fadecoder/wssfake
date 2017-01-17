#!/bin/bash

spec=$1

xenstore-write /local/domain/1/wzg/signal "prepare"

xm vcpu-set 0 1
xm vcpu-pin 0 0 1
xm vcpu-pin 1 0 1
/common/mb/meb/xc -xt
/common/mb/meb/xc -h 1 8192

#./xc -r 1
ssh 172.17.11.51 "/root/run_benchmarks/run_test.sh $spec> /dev/null 2>&1 &"
cnt=0
while [[ 1 == 1 ]]; do
	/common/mb/meb/xc -r 1
	sleep 60
	cnt=$[ $cnt + 1 ]
	program=`xenstore-read /local/domain/1/wzg/signal`
	echo program=$program
	echo cnt=$cnt
	if [[ "$program" == "finished" ]];
	then 
	    break
	fi
	if [[ $program == "begin" ]];
	then
	    /common/mb/meb/xc -l 1 > $spec.$cnt.txt
	fi
#	m1=`xenstore-read /local/domain/2/vmm/mem_total`
#	((m1=m1/1024))
done
#echo ~~~~
#echo s=$s
#echo c=$c
#echo a=$a
#xenstore-write /local/domain/1/vmm/a $a
