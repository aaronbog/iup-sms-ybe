#!/bin/bash

cd cadical-master
rm -r ./build/
./configure -l
cd ./build/
make
cd ../../ybe-sms/
rm -r ./build/
cmake . -B./build
cd ./build/
make