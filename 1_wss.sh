low=60
high=200
step=100
last_time=2
round=20
function run(){
        xenstore-write /local/domain/1/vmm/wss_signal "begin"
        if (($#==1));then
#            echo ./fake $1 $last_time
            ./fake $1 $last_time
            wss=$1
        else
#            echo ./fake $low $high $last_time
            wss=`./fake $low $high $last_time`
        fi
        echo $wss
        xenstore-write /local/domain/1/vmm/wss $wss
        xenstore-write /local/domain/1/vmm/wss_signal "finished"
        sleep 1
}
function momo(){
        xenstore-write /local/domain/1/vmm/wss_signal_all "begin_all"
        for((wss=low;wss<high;wss=wss+step));do
            run $wss
        done;
        for((wss=high;wss>=low;wss=wss-step));do
            run $wss
        done;
        xenstore-write /local/domain/1/vmm/wss_signal_all "finished_all"
}
function random(){
    xenstore-write /local/domain/1/vmm/wss_signal_all "begin_all"
    for((i=0;i<round;i++));do
        run 
    done;
    xenstore-write /local/domain/1/vmm/wss_signal_all "finished_all"
    
}
random
