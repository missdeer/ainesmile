#!/bin/sh
cd 3rdparty/scintilla/qt/ScintillaEdit/
python WidgetGen.py
cd ../../../..
qmake -r CONFIG+=release
hg log -l 1 | head -n 1 | awk '{print $2}' > REVSION
date > DATE
make -j 4
