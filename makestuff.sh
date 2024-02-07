#!/bin/bash

cd cadical-user-propagation
rm -r ./build/
./configure
cd ./build/
make
cd ../../ybe-sms/
rm -r ./build/
cmake . -B./build
cd ./build/
make