#!/bin/sh
./pre-build.sh
qmake -r CONFIG+=release
make -j 4
