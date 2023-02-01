#!/bin/sh
./pre-build.sh
qmake -r CONFIG+=debug 
make -j 4
