#!/bin/bash

processName=gp-cred

processID=$(ps -e | grep $processName | awk '{print $1}'); 
echo "Monitoring process: $processID"

maxMem=1


while [ 1 ] ; do

    memNow=$(cat /proc/$processID/status | grep "VmSize" | awk '{print $2}')

    if [ -z $memNow ] ; then 
        break;
    fi
    
    if [ "$memNow" -gt "$maxMem" ] ; then
            maxMem=$memNow
    fi

    echo $memNow , $maxMem
done

echo "Program $processName used up to $maxMem"

