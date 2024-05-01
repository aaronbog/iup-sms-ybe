#!/bin/bash

cd cadical-user-propagation
rm -r ./build/
./configure -l --profile
cd ./build/
make
cd ../../ybe-sms/
rm -r ./build/
cmake -DCMAKE_CXX_FLAGS=-pg -DCMAKE_EXE_LINKER_FLAGS=-pg -DCMAKE_SHARED_LINKER_FLAGS=-pg . -B./build
cd ./build/
make
