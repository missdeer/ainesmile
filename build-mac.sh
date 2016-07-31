#!/bin/sh
BOOST_ROOT=/usr/local/include
export BOOST_ROOT
BOOST_LIBS=/usr/local/lib
export BOOST_LIBS
PATH=$HOME/Qt/5.7/clang_64/bin:$PATH
export PATH
./build.sh mac
