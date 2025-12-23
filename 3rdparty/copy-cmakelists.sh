#!/bin/sh
cp cmakes/CMakeLists.txt.lexilla lexilla/CMakeLists.txt
cp cmakes/CMakeLists.txt.scintilla scintilla/CMakeLists.txt
cd lexilla
git apply ../lexilla.patch
