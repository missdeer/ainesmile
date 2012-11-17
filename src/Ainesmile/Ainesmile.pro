#-------------------------------------------------
#
# Project created by QtCreator 2012-06-30T16:38:21
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): {
    QT += widgets webkitwidgets
    QT -= webkit
}

TARGET = Ainesmile
TEMPLATE = app


INCLUDEPATH += CodeEdit scintilla/qt/ScintillaEditBase scintilla/include scintilla/src scintilla/lexlib

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1 LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT

SOURCES += main.cpp\
        mainwindow.cpp \
    scintilla/src/XPM.cxx \
    scintilla/src/ViewStyle.cxx \
    scintilla/src/UniConversion.cxx \
    scintilla/src/Style.cxx \
    scintilla/src/Selection.cxx \
    scintilla/src/ScintillaBase.cxx \
    scintilla/src/RunStyles.cxx \
    scintilla/src/RESearch.cxx \
    scintilla/src/PositionCache.cxx \
    scintilla/src/PerLine.cxx \
    scintilla/src/LineMarker.cxx \
    scintilla/src/KeyMap.cxx \
    scintilla/src/Indicator.cxx \
    scintilla/src/ExternalLexer.cxx \
    scintilla/src/Editor.cxx \
    scintilla/src/Document.cxx \
    scintilla/src/Decoration.cxx \
    scintilla/src/ContractionState.cxx \
    scintilla/src/CharClassify.cxx \
    scintilla/src/CellBuffer.cxx \
    scintilla/src/Catalogue.cxx \
    scintilla/src/CallTip.cxx \
    scintilla/src/AutoComplete.cxx \
    scintilla/lexers/LexYAML.cxx \
    scintilla/lexers/LexVisualProlog.cxx \
    scintilla/lexers/LexVHDL.cxx \
    scintilla/lexers/LexVerilog.cxx \
    scintilla/lexers/LexVB.cxx \
    scintilla/lexers/LexTxt2tags.cxx \
    scintilla/lexers/LexTeX.cxx \
    scintilla/lexers/LexTCMD.cxx \
    scintilla/lexers/LexTCL.cxx \
    scintilla/lexers/LexTAL.cxx \
    scintilla/lexers/LexTADS3.cxx \
    scintilla/lexers/LexTACL.cxx \
    scintilla/lexers/LexSQL.cxx \
    scintilla/lexers/LexSpice.cxx \
    scintilla/lexers/LexSpecman.cxx \
    scintilla/lexers/LexSorcus.cxx \
    scintilla/lexers/LexSML.cxx \
    scintilla/lexers/LexSmalltalk.cxx \
    scintilla/lexers/LexScriptol.cxx \
    scintilla/lexers/LexRuby.cxx \
    scintilla/lexers/LexRebol.cxx \
    scintilla/lexers/LexR.cxx \
    scintilla/lexers/LexPython.cxx \
    scintilla/lexers/LexPS.cxx \
    scintilla/lexers/LexProgress.cxx \
    scintilla/lexers/LexPowerShell.cxx \
    scintilla/lexers/LexPowerPro.cxx \
    scintilla/lexers/LexPOV.cxx \
    scintilla/lexers/LexPLM.cxx \
    scintilla/lexers/LexPerl.cxx \
    scintilla/lexers/LexPB.cxx \
    scintilla/lexers/LexPascal.cxx \
    scintilla/lexers/LexOthers.cxx \
    scintilla/lexers/LexOScript.cxx \
    scintilla/lexers/LexOpal.cxx \
    scintilla/lexers/LexNsis.cxx \
    scintilla/lexers/LexNimrod.cxx \
    scintilla/lexers/LexMySQL.cxx \
    scintilla/lexers/LexMSSQL.cxx \
    scintilla/lexers/LexMPT.cxx \
    scintilla/lexers/LexModula.cxx \
    scintilla/lexers/LexMMIXAL.cxx \
    scintilla/lexers/LexMetapost.cxx \
    scintilla/lexers/LexMatlab.cxx \
    scintilla/lexers/LexMarkdown.cxx \
    scintilla/lexers/LexMagik.cxx \
    scintilla/lexers/LexLua.cxx \
    scintilla/lexers/LexLout.cxx \
    scintilla/lexers/LexLisp.cxx \
    scintilla/lexers/LexKix.cxx \
    scintilla/lexers/LexInno.cxx \
    scintilla/lexers/LexHTML.cxx \
    scintilla/lexers/LexHaskell.cxx \
    scintilla/lexers/LexGui4Cli.cxx \
    scintilla/lexers/LexGAP.cxx \
    scintilla/lexers/LexFortran.cxx \
    scintilla/lexers/LexForth.cxx \
    scintilla/lexers/LexFlagship.cxx \
    scintilla/lexers/LexEScript.cxx \
    scintilla/lexers/LexErlang.cxx \
    scintilla/lexers/LexEiffel.cxx \
    scintilla/lexers/LexECL.cxx \
    scintilla/lexers/LexD.cxx \
    scintilla/lexers/LexCSS.cxx \
    scintilla/lexers/LexCsound.cxx \
    scintilla/lexers/LexCrontab.cxx \
    scintilla/lexers/LexCPP.cxx \
    scintilla/lexers/LexConf.cxx \
    scintilla/lexers/LexCoffeeScript.cxx \
    scintilla/lexers/LexCOBOL.cxx \
    scintilla/lexers/LexCmake.cxx \
    scintilla/lexers/LexCLW.cxx \
    scintilla/lexers/LexCaml.cxx \
    scintilla/lexers/LexBullant.cxx \
    scintilla/lexers/LexBasic.cxx \
    scintilla/lexers/LexBash.cxx \
    scintilla/lexers/LexBaan.cxx \
    scintilla/lexers/LexAVS.cxx \
    scintilla/lexers/LexAVE.cxx \
    scintilla/lexers/LexAU3.cxx \
    scintilla/lexers/LexASY.cxx \
    scintilla/lexers/LexAsn1.cxx \
    scintilla/lexers/LexAsm.cxx \
    scintilla/lexers/LexAPDL.cxx \
    scintilla/lexers/LexAda.cxx \
    scintilla/lexers/LexAbaqus.cxx \
    scintilla/lexers/LexA68k.cxx \
    scintilla/lexlib/WordList.cxx \
    scintilla/lexlib/StyleContext.cxx \
    scintilla/lexlib/PropSetSimple.cxx \
    scintilla/lexlib/LexerSimple.cxx \
    scintilla/lexlib/LexerNoExceptions.cxx \
    scintilla/lexlib/LexerModule.cxx \
    scintilla/lexlib/LexerBase.cxx \
    scintilla/lexlib/CharacterSet.cxx \
    scintilla/lexlib/Accessor.cxx \
    scintilla/qt/ScintillaEditBase/ScintillaQt.cpp \
    scintilla/qt/ScintillaEditBase/ScintillaEditBase.cpp \
    scintilla/qt/ScintillaEditBase/PlatQt.cpp \
    CodeEdit/codeeditpage.cpp \
    tabwidget.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    scintilla/src/XPM.h \
    scintilla/src/ViewStyle.h \
    scintilla/src/UniConversion.h \
    scintilla/src/SVector.h \
    scintilla/src/Style.h \
    scintilla/src/SplitVector.h \
    scintilla/src/Selection.h \
    scintilla/src/ScintillaBase.h \
    scintilla/src/RunStyles.h \
    scintilla/src/RESearch.h \
    scintilla/src/PositionCache.h \
    scintilla/src/PerLine.h \
    scintilla/src/Partitioning.h \
    scintilla/src/LineMarker.h \
    scintilla/src/KeyMap.h \
    scintilla/src/Indicator.h \
    scintilla/src/FontQuality.h \
    scintilla/src/ExternalLexer.h \
    scintilla/src/Editor.h \
    scintilla/src/Document.h \
    scintilla/src/Decoration.h \
    scintilla/src/ContractionState.h \
    scintilla/src/CharClassify.h \
    scintilla/src/CellBuffer.h \
    scintilla/src/Catalogue.h \
    scintilla/src/CallTip.h \
    scintilla/src/AutoComplete.h \
    scintilla/include/ScintillaWidget.h \
    scintilla/include/Scintilla.h \
    scintilla/include/SciLexer.h \
    scintilla/include/Platform.h \
    scintilla/include/ILexer.h \
    scintilla/lexlib/WordList.h \
    scintilla/lexlib/StyleContext.h \
    scintilla/lexlib/SparseState.h \
    scintilla/lexlib/PropSetSimple.h \
    scintilla/lexlib/OptionSet.h \
    scintilla/lexlib/LexerSimple.h \
    scintilla/lexlib/LexerNoExceptions.h \
    scintilla/lexlib/LexerModule.h \
    scintilla/lexlib/LexerBase.h \
    scintilla/lexlib/LexAccessor.h \
    scintilla/lexlib/CharacterSet.h \
    scintilla/lexlib/Accessor.h \
    scintilla/qt/ScintillaEditBase/ScintillaQt.h \
    scintilla/qt/ScintillaEditBase/ScintillaEditBase.h \
    scintilla/qt/ScintillaEditBase/PlatQt.h \
    CodeEdit/codeeditpage.h \
    tabwidget.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui

OTHER_FILES += \
    Ainesmile.rc

RESOURCES += \
    Ainesmile.qrc

# Windows icons
RC_FILE = Ainesmile.rc


# Mac OS X icon
macx: {
QT += opengl

ICON = rc/Ainesmile.icns
icon.files += rc/Ainesmile.png
INSTALLS = target \
    desktop \
    icon
}
