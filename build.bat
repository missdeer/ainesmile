cd 3rdparty\scintilla\qt\ScintillaEdit\
python WidgetGen.py
cd ..\..\..\..
hg log -l 1 | head -n 1 | gawk '{print $2}' > src\ainesmile\REVISION
date /T > src\ainesmile\DATE
time /T >> src\ainesmile\DATE
qmake -r CONFIG+=release
mingw32-make -j 4
copy /y 3rdparty\scintilla\bin\ScintillaEdit3.dll src\ainesmile\release\ScintillaEdit3.dll
