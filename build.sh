#!/bin/sh
cd 3rdparty/scintilla/qt/ScintillaEdit/
python WidgetGen.py
cd ../../../..
qmake -r CONFIG+=release
git log -n 1 | head -n 1 | awk '{print $2}' > src/ainesmile/REVISION
if [ "$1" = "mac" ]; then
    date > src/ainesmile/DATE
else
    date -R  > src/ainesmile/DATE
fi
make -j 4
