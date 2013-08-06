#!/bin/sh
mkdir src/ainesmile/ainesmile.app/Contents/Frameworks
cp -r 3rdparty/scintilla/bin/ScintillaEdit.framework src/ainesmile/ainesmile.app/Contents/Frameworks/
mkdir src/ainesmile/ainesmile.app/Contents/Resource
cp -r resource/* src/ainesmile/ainesmile.app/Contents/Resource/
cp resource/.ainesmilerc src/ainesmile/ainesmile.app/Contents/Resource/
