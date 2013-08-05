greaterThan(QT_MAJOR_VERSION, 4): cache()
TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    3rdparty/scintilla/qt/ScintillaEditBase \
    3rdparty/scintilla/qt/ScintillaEdit \
    src/utility \
    src/ainesmile 
