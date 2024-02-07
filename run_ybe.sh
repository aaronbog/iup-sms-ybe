#!/bin/bash

./ybe-sms/build/src/diag_maker -s 5

input="./diagonals.txt"
while read -r diag
do
    ./ybe-sms/build/src/ybe_sms -s 5 --allModels --checkSols --checkFreq 40 --diagPart --diag "$diag" --v2 > out_"$diag".txt
done < "$input"

#./iup-sms-ybe/ybe-sms/build/src/ybe_sms -s 4 --allModels --checkSols --checkFreq 40 --diagPart --v2 > out.txt