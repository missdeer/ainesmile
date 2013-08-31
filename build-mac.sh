#!/bin/sh
BOOST_ROOT=/opt/local/include
export BOOST_ROOT
BOOST_LIBS=/opt/local/lib
export BOOST_LIBS
PATH=/Users/yangfan/Qt5.1.1/5.1.1/clang_64/bin:$PATH
export PATH
./build.sh mac
