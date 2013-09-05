#!/bin/sh
BOOST_ROOT=/opt/local/include
export BOOST_ROOT
BOOST_LIBS=/opt/local/lib
export BOOST_LIBS
PATH=/usr/local/Trolltech/Qt-4.8.5/bin:$PATH
export PATH
./build.sh mac
