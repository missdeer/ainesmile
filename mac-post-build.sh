#!/bin/sh
mkdir src/ainesmile/ainesmile.app/Contents/Frameworks
cp -R 3rdparty/scintilla/bin/ScintillaEdit.framework src/ainesmile/ainesmile.app/Contents/Frameworks/
cp resource/*.xml src/ainesmile/ainesmile.app/Contents/Resources/
cp -r resource/language src/ainesmile/ainesmile.app/Contents/Resources/
cp -r resource/MacOSX/themes src/ainesmile/ainesmile.app/Contents/Resources/
cp resource/.ainesmilerc src/ainesmile/ainesmile.app/Contents/Resources/
