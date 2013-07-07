cd 3rdparty\scintilla\qt\ScintillaEdit\
python WidgetGen.py
cd ..\..\..\..
qmake -r CONFIG+=release
mingw32-make -j 4
