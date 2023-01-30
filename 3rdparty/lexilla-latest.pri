INCLUDEPATH += $$PWD/lexilla/include \
    $$PWD/lexilla/src \
    $$PWD/lexilla/lexlib

SOURCES += \
    $$files($$PWD/lexilla/src/*.cxx, false)  \
    $$files($$PWD/lexilla/lexlib/*.cxx, false)  \
    $$files($$PWD/lexilla/lexers/*.cxx, false)

HEADERS += \
    $$files($$PWD/lexilla/include/*.h, false)

RC_FILE += $$PWD/lexilla/src/LexillaVersion.rc

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}
