#!/bin/sh
BOOST_ROOT=/opt/local/include
export BOOST_ROOT
BOOST_LIBS=/opt/local/lib
export BOOST_LIBS
PATH=$HOME/Qt5.5/5.5.0/clang_64/bin:$PATH
export PATH
./build.sh mac
