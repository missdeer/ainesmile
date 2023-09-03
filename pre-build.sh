#!/bin/bash
cd 3rdparty/scintilla/qt/ScintillaEdit/
python_path=$(which python)
echo $python_path
if [ "$python_path"x = ""x ]; then
	python3 WidgetGen.py
else
	python WidgetGen.py
fi
cd ../../../..
git rev-parse --short HEAD >src/ainesmile/REVISION
if [ "$1" = "mac" ]; then
	date >src/ainesmile/DATE
else
	date -R >src/ainesmile/DATE
fi
