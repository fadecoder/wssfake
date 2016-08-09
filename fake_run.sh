#!/bin/sh

majorpf_0=`grep pgmajfault /proc/vmstat | awk '{print $2}'`

begin="Begin: `date`"

trap "xenstore-write vmm/signal finished;xenstore-write vmm/program '';exit " INT TERM EXIT
xenstore-write vmm/signal begin


xenstore-write vmm/program "mcf"
./fake 300 100

xenstore-write vmm/program ""
xenstore-write vmm/signal finished

echo "$begin,   end: `date`"
majorpf_1=`grep pgmajfault /proc/vmstat | awk '{print $2}'`
sum_majorpf=$[$majorpf_1 - $majorpf_0]
echo "Major Pafe faults $sum_majorpf"
