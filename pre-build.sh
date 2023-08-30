#!/bin/sh
cd 3rdparty/scintilla/qt/ScintillaEdit/
python WidgetGen.py
cd ../../../..
git rev-parse --short HEAD > src/ainesmile/REVISION
if [ "$1" = "mac" ]; then
    date > src/ainesmile/DATE
else
    date -R  > src/ainesmile/DATE
fi
