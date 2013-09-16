#!/bin/sh
mkdir src/ainesmile/ainesmile.app/Contents/Frameworks
cp -R 3rdparty/scintilla/bin/ScintillaEdit.framework src/ainesmile/ainesmile.app/Contents/Frameworks/
mkdir src/ainesmile/ainesmile.app/Contents/Resource
cp resource/*.xml src/ainesmile/ainesmile.app/Contents/Resource/
cp -r resource/languages src/ainesmile/ainesmile.app/Contents/Resource/
cp -r resource/MacOSX/themes src/ainesmile/ainesmile.app/Contents/Resource/
cp resource/.ainesmilerc src/ainesmile/ainesmile.app/Contents/Resource/
