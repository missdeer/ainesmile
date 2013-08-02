#-------------------------------------------------
#
# Project created by QtCreator 2012-06-30T16:38:21
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): {
    QT += widgets
}

TARGET = ainesmile
TEMPLATE = app

include(../../Boost.pri)

INCLUDEPATH += CodeEdit \
    ../../3rdparty/rapidxml-1.13 \
    ../../3rdparty/scintilla/qt/ScintillaEditBase \
    ../../3rdparty/scintilla/qt/ScintillaEdit \
    ../../3rdparty/scintilla/include \
    ../../3rdparty/scintilla/src \
    ../../3rdparty/scintilla/lexlib

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1 LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT


SOURCES += main.cpp\
    CodeEdit/codeeditpage.cpp \
    CodeEdit/scintillaconfig.cpp \
    mainwindow.cpp \
    tabwidget.cpp \
    aboutdialog.cpp \
    gotolinedialog.cpp \
    config.cpp

HEADERS  += mainwindow.h \
    CodeEdit/codeeditpage.h \
    CodeEdit/scintillaconfig.h \
    tabwidget.h \
    aboutdialog.h \
    gotolinedialog.h \
    config.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    gotolinedialog.ui

OTHER_FILES += \
    ainesmile.rc

RESOURCES += \
    ainesmile.qrc

# Windows icons
RC_FILE = ainesmile.rc

win32: {
LIBS += -L ../../3rdparty/scintilla/bin -lScintillaEdit3
}
unix: !macx: {
LIBS += -L ../../3rdparty/scintilla/bin -lScintillaEdit
}

# Mac OS X icon
macx: {
QT += opengl
LIBS += -F ../../3rdparty/scintilla/bin -framework ScintillaEdit 
ICON = rc/ainesmile.icns
icon.files += rc/ainesmile.png
INSTALLS = target \
    desktop \
    icon
}
