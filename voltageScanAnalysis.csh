#!/bin/bash
#This script is meant to autonomously run the voltageScan.C macro over all the output files generated by the
#voltageScan.sh bash script.
#-AH


count_voltage=67500
set titleVoltage=${voltage}

filename=baseline-67500mV/*.dat
holder=$(echo $filename)
for chan in {1..4}
do
    root -l -q -b 'timeDiffAnalysis.C('\"$holder\"',0,'"${count_voltage}","${chan}"')'
done

for chan in {1..4}
do
    for offset in {1..5}
    do
	inc=$(expr ${offset} \* 100)
	new_voltage=$(expr ${count_voltage} + ${inc})
	filename=CH${chan}-${new_voltage}mV/*dat
	holder=$(echo $filename)
	root -l -q -b 'timeDiffAnalysis.C('\"$holder\"',0,'"${new_voltage}","${chan}"')'
    done
done




