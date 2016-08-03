cache()
TEMPLATE = subdirs

CONFIG += ordered

AINESMILE_VERSION = 1.0.0

contains(QT_ARCH, i386): ARCHITECTURE = x86
else: ARCHITECTURE = $$QT_ARCH

macx: PLATFORM = "mac"
else:win32: PLATFORM = "windows"
else:linux-*: PLATFORM = "linux-$${ARCHITECTURE}"
else: PLATFORM = "unknown"

SUBDIRS += \
    3rdparty/scintilla/qt/ScintillaEdit \
    src/ainesmile 

win32-msvc* {
    #Don't warn about sprintf, fopen etc being 'unsafe'
    DEFINES += _CRT_SECURE_NO_WARNINGS
}
