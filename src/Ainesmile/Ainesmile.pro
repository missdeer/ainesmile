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
    mainwindow.cpp \
    tabwidget.cpp \
    aboutdialog.cpp \
    gotolinedialog.cpp

HEADERS  += mainwindow.h \
    CodeEdit/codeeditpage.h \
    tabwidget.h \
    aboutdialog.h \
    gotolinedialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    gotolinedialog.ui

OTHER_FILES += \
    Ainesmile.rc

RESOURCES += \
    Ainesmile.qrc

# Windows icons
RC_FILE = Ainesmile.rc

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
ICON = rc/Ainesmile.icns
icon.files += rc/Ainesmile.png
INSTALLS = target \
    desktop \
    icon
}
