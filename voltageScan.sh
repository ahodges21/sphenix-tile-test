#!/bin/bash
mkdir /data1/voltageScan_30min
cd /data1/voltageScan_30min
mkdir baseline-67500mV

#initialize all channels to same voltage for beginning of test and take baseline test 
curl -X POST 192.168.10.213/api/hardware/modules/2/set-voltage?voltage=68.0
echo
curl -X POST 192.168.10.213/api/hardware/modules/3/set-voltage?voltage=68.0
echo
curl -X POST 192.168.10.213/api/hardware/modules/4/set-voltage?voltage=68.0
echo
curl -X POST 192.168.10.213/api/hardware/modules/14/set-voltage?voltage=68.0
echo
~/drsLog/drsLog 5.0 0.45 60.0 R AND 11110 0.05 0.05 0.05 0.05 1000000 1800 ./baseline-67500mV > baseline-67500mV/drs4.log

#set voltage change in between runs
voltDiff=0.1			    


for i in {1..4}
do
    offset=0
    runVoltage=67500
    let titleVoltage=$runVoltage
     
    

    if [ $i = 1 ];
    then
	gpio=4
    fi
    if [ $i = 2 ];
    then
	gpio=3
    fi
    if [ $i = 3 ];
    then
	gpio=2
    fi
    if [ $i = 4 ];
    then
	gpio=14
    fi
    for k in {1..5}
    do
	offset=$(expr ${k} \* 100)
	let titleVoltage=$(expr ${runVoltage} + ${offset})
        curlVoltage=$(echo "scale = 3;  $titleVoltage / 1000"| bc) 
	mkdir CH${i}-${titleVoltage}mV
	curl -X POST 192.168.10.213/api/hardware/modules/${gpio}/set-voltage?voltage=${curlVoltage}
	printf "\nStarting ${curlVoltage}V run \n"
	~/drsLog/drsLog 5.0 0.45 60.0 R AND 11110 0.05 0.05 0.05 0.05 1000000 1800 ./CH${i}-${titleVoltage}mV > CH${i}-${titleVoltage}mV/drs4.log
	

	
    done
done



