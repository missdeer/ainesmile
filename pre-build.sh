#!/bin/sh
cd 3rdparty/scintilla/qt/ScintillaEdit/
python3 WidgetGen.py
cd ../../../..
git log -n 1 | head -n 1 | awk '{print $2}' > src/ainesmile/REVISION
if [ "$1" = "mac" ]; then
    date > src/ainesmile/DATE
else
    date -R  > src/ainesmile/DATE
fi
