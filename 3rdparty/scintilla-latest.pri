QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
equals(QT_MAJOR_VERSION, 6): QT += core5compat

CONFIG += c++17

VERSION = 5.3.2

SOURCES += \
    $$files($$PWD/scintilla/qt/ScintillaEdit/*.cpp, false)  \
    $$files($$PWD/scintilla/qt/ScintillaEditBase/*.cpp, false) \
    $$PWD/scintilla/src/ChangeHistory.cxx \
    $$PWD/scintilla/src/XPM.cxx \
    $$PWD/scintilla/src/ViewStyle.cxx \
    $$PWD/scintilla/src/UniqueString.cxx \
    $$PWD/scintilla/src/UniConversion.cxx \
    $$PWD/scintilla/src/Style.cxx \
    $$PWD/scintilla/src/Selection.cxx \
    $$PWD/scintilla/src/ScintillaBase.cxx \
    $$PWD/scintilla/src/RunStyles.cxx \
    $$PWD/scintilla/src/RESearch.cxx \
    $$PWD/scintilla/src/PositionCache.cxx \
    $$PWD/scintilla/src/PerLine.cxx \
    $$PWD/scintilla/src/MarginView.cxx \
    $$PWD/scintilla/src/LineMarker.cxx \
    $$PWD/scintilla/src/KeyMap.cxx \
    $$PWD/scintilla/src/Indicator.cxx \
    $$PWD/scintilla/src/Geometry.cxx \
    $$PWD/scintilla/src/EditView.cxx \
    $$PWD/scintilla/src/Editor.cxx \
    $$PWD/scintilla/src/EditModel.cxx \
    $$PWD/scintilla/src/Document.cxx \
    $$PWD/scintilla/src/Decoration.cxx \
    $$PWD/scintilla/src/DBCS.cxx \
    $$PWD/scintilla/src/ContractionState.cxx \
    $$PWD/scintilla/src/CharClassify.cxx \
    $$PWD/scintilla/src/CharacterType.cxx \
    $$PWD/scintilla/src/CharacterCategoryMap.cxx \
    $$PWD/scintilla/src/CellBuffer.cxx \
    $$PWD/scintilla/src/CaseFolder.cxx \
    $$PWD/scintilla/src/CaseConvert.cxx \
    $$PWD/scintilla/src/CallTip.cxx \
    $$PWD/scintilla/src/AutoComplete.cxx

HEADERS += \
    $$files($$PWD/scintilla/src/*.h, false)  \
    $$files($$PWD/scintilla/qt/ScintillaEdit/*.h, false)  \
    $$files($$PWD/scintilla/qt/ScintillaEditBase/*.h, false)

INCLUDEPATH += $$PWD/scintilla/qt/ScintillaEdit \
    $$PWD/scintilla/qt/ScintillaEditBase \
    $$PWD/scintilla/include \
    $$PWD/scintilla/src

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1 EXPORT_IMPORT_API=
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}

macx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
}
