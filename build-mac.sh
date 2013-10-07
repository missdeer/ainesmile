#!/bin/sh
BOOST_ROOT=/opt/local/include
export BOOST_ROOT
BOOST_LIBS=/opt/local/lib
export BOOST_LIBS
PATH=/usr/local/Qt-5.2.0/bin:$PATH
export PATH
./build.sh mac
