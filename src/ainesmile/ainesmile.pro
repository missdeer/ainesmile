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
    Dialogs \
    ../../3rdparty/rapidxml-1.13 \
    ../../3rdparty/scintilla/qt/ScintillaEditBase \
    ../../3rdparty/scintilla/qt/ScintillaEdit \
    ../../3rdparty/scintilla/include \
    ../../3rdparty/scintilla/src \
    ../../3rdparty/scintilla/lexlib

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1 LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT


SOURCES += main.cpp\
    mainwindow.cpp \
    tabwidget.cpp \
    config.cpp \
    CodeEdit/codeeditpage.cpp \
    CodeEdit/scintillaconfig.cpp \
    Dialogs/aboutdialog.cpp \
    Dialogs/gotolinedialog.cpp \
    Dialogs/findreplacedialog.cpp \
    Dialogs/registerdialog.cpp \
    Dialogs/preferencedialog.cpp \
    stupidcheck.cpp \
    Dialogs/nagdialog.cpp \
    CodeEdit/recentfiles.cpp \
    Dialogs/summarydialog.cpp

HEADERS  += mainwindow.h \
    tabwidget.h \
    config.h \
    CodeEdit/codeeditpage.h \
    CodeEdit/scintillaconfig.h \
    Dialogs/aboutdialog.h \
    Dialogs/gotolinedialog.h \
    Dialogs/findreplacedialog.h \
    Dialogs/registerdialog.h \
    Dialogs/preferencedialog.h \
    stupidcheck.h \
    Dialogs/nagdialog.h \
    CodeEdit/recentfiles.h \
    Dialogs/summarydialog.h

FORMS    += mainwindow.ui \
    Dialogs/aboutdialog.ui \
    Dialogs/gotolinedialog.ui \
    Dialogs/findreplacedialog.ui \
    Dialogs/registerdialog.ui \
    Dialogs/preferencedialog.ui \
    Dialogs/nagdialog.ui \
    Dialogs/summarydialog.ui

OTHER_FILES += \
    ainesmile.rc

RESOURCES += \
    ainesmile.qrc

# Windows icons
RC_FILE = ainesmile.rc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../utility/release/ -lutility
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../utility/debug/ -lutility
else:unix: LIBS += -L$$OUT_PWD/../utility/ -lutility

INCLUDEPATH += $$PWD/../utility
DEPENDPATH += $$PWD/../utility

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../utility/release/utility.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../utility/debug/utility.lib
#else:unix: PRE_TARGETDEPS += $$OUT_PWD/../utility/libutility.a

win32: {
INCLUDEPATH += $$(PORTED)\include
LIBS += -L..\..\3rdparty\scintilla\bin -lScintillaEdit3 -L"$$(PORTED)\lib" -lIphlpapi
}
unix: !macx: {
LIBS += -L ../../3rdparty/scintilla/bin -lScintillaEdit
}

# Mac OS X icon
macx: {
QT += opengl
LIBS += -F ../../3rdparty/scintilla/bin -framework ScintillaEdit \
    -framework CoreFoundation \
    -framework IOKit

ICON = rc/ainesmile.icns
icon.files += rc/ainesmile.png
INSTALLS = target \
    desktop \
    icon
}

LIBS +=  -lssl -lcrypto
