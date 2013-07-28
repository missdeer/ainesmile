cd 3rdparty\scintilla\qt\ScintillaEdit\
python WidgetGen.py
cd ..\..\..\..
hg log -l 1 | head -n 1 | gawk '{print $2}' > src\Ainesmile\REVISION
date /T > src\Ainesmile\DATE
time /T >> src\Ainesmile\DATE
qmake -r CONFIG+=release
mingw32-make -j 4
copy /y 3rdparty\scintilla\bin\ScintillaEdit3.dll src\Ainesmile\release\ScintillaEdit3.dll