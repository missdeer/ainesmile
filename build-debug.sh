#!/bin/sh
cd 3rdparty/scintilla/qt/ScintillaEdit/
python WidgetGen.py
cd ../../../..
qmake -r CONFIG+=debug 
git log -n 1 | head -n 1 | awk '{print $2}' > src/ainesmile/REVISION
date  > src/ainesmile/DATE
make -j 4
