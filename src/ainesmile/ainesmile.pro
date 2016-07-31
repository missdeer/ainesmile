#-------------------------------------------------
#
# Project created by QtCreator 2012-06-30T16:38:21
#
#-------------------------------------------------

QT       += core gui xml network widgets webenginewidgets

CONFIG += c++11

TARGET = ainesmile
TEMPLATE = app

include(../../Boost.pri)
include(../../3rdparty/qtsingleapplication/qtsingleapplication.pri)


PATTERN = $${PLATFORM}$(INSTALL_EDITION)-$${AINESMILE_VERSION}$(INSTALL_POSTFIX)

macx:INSTALLER_NAME = "ainesmile-$${QTCREATOR_VERSION}"
else:INSTALLER_NAME = "ainesmile-$${PATTERN}"

PRECOMPILED_HEADER = pch.h

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
    Dialogs/preferencedialog.cpp \
    CodeEdit/recentfiles.cpp \
    Dialogs/summarydialog.cpp \
    Dialogs/windowlistdialog.cpp \
    Dialogs/selectedlistview.cpp

HEADERS  += mainwindow.h \
    tabwidget.h \
    config.h \
    CodeEdit/codeeditpage.h \
    CodeEdit/scintillaconfig.h \
    Dialogs/aboutdialog.h \
    Dialogs/gotolinedialog.h \
    Dialogs/preferencedialog.h \
    CodeEdit/recentfiles.h \
    Dialogs/summarydialog.h \
    Dialogs/windowlistdialog.h \
    Dialogs/selectedlistview.h \
    pch.h

FORMS    += mainwindow.ui \
    Dialogs/aboutdialog.ui \
    Dialogs/gotolinedialog.ui \
    Dialogs/preferencedialog.ui \
    Dialogs/summarydialog.ui \
    Dialogs/windowlistdialog.ui

OTHER_FILES += \
    ainesmile.rc \
    ../../3rdparty/qtsingleapplication/qtsingleapplication.pri \
    ../../3rdparty/qtsingleapplication/qtsinglecoreapplication.pri

RESOURCES += \
    ainesmile.qrc

# Windows icons
RC_FILE = ainesmile.rc

LIBS +=  -lssl -lcrypto

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
LIBS += -L..\..\3rdparty\scintilla\bin -lScintillaEdit3  -L"$$(PORTED)\lib" -lIphlpapi -luser32
!gcc: {
LIBS -= -lssl -lcrypto 
contains(QMAKE_HOST.arch, x86_64):{
LIBS += -L..\..\3rdparty\winopenssl_1_0_1f_x64\lib -lssleay32 -llibeay32 
}
else : {
LIBS += -L..\..\3rdparty\winopenssl_1_0_1f_x86\lib -lssleay32 -llibeay32 
}
}
}
unix: !macx: {
LIBS += -L ../../3rdparty/scintilla/bin -lScintillaEdit
}

# Mac OS X icon
macx: {
    # this should be very temporary:
    MENU_NIB = $$(MENU_NIB_FILE)
    isEmpty(MENU_NIB): MENU_NIB = "FATAT_SET_MENU_NIB_FILE_ENV"
    copy_menu_nib_installer.commands = cp -R \"$$MENU_NIB\" \"$${INSTALLER_NAME}.app/Contents/Resources\"

    codesign_installer.commands = codesign -s \"$(SIGNING_IDENTITY)\" $(SIGNING_FLAGS) \"$${INSTALLER_NAME}.app\"
    dmg_installer.commands = hdiutil create -srcfolder "$${INSTALLER_NAME}.app" -volname \"Qt Creator\" -format UDBZ "ainesmile-$${PATTERN}-installer.dmg" -ov -scrub -stretch 2g
    QMAKE_EXTRA_TARGETS += codesign_installer dmg_installer copy_menu_nib_installer

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

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = translations/ainesmile_en_US.ts \
                  translations/ainesmile_zh_CN.ts
