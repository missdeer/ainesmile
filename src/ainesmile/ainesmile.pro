#-------------------------------------------------
#
# Project created by QtCreator 2012-06-30T16:38:21
#
#-------------------------------------------------

QT       += core gui xml network widgets webenginewidgets

CONFIG += c++11 precompile_header

TARGET = ainesmile
TEMPLATE = app

include($$PWD/../../Boost.pri)
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/scintilla/qt/ScintillaEdit/ScintillaEdit.pri)


PATTERN = $${PLATFORM}$(INSTALL_EDITION)-$${AINESMILE_VERSION}$(INSTALL_POSTFIX)

macx:INSTALLER_NAME = "ainesmile-$${QTCREATOR_VERSION}"
else:INSTALLER_NAME = "ainesmile-$${PATTERN}"

PRECOMPILED_HEADER = stdafx.h

INCLUDEPATH += $$PWD/CodeEdit \
    $$PWD/Dialogs \
    $$PWD/uchardet \
    $$PWD/../../3rdparty/scintilla/qt/ScintillaEditBase \
    $$PWD/../../3rdparty/scintilla/qt/ScintillaEdit \
    $$PWD/../../3rdparty/scintilla/include \
    $$PWD/../../3rdparty/scintilla/src \
    $$PWD/../../3rdparty/scintilla/lexlib

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1 LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT


SOURCES += $$PWD/main.cpp\
    $$PWD/mainwindow.cpp \
    $$PWD/tabwidget.cpp \
    $$PWD/config.cpp \
    $$PWD/CodeEdit/*.cpp \
    $$PWD/Dialogs/*.cpp \
    $$PWD/uchardet/*.cpp

HEADERS  += $$PWD/mainwindow.h \
    $$PWD/tabwidget.h \
    $$PWD/config.h \
    $$PWD/CodeEdit/*.h \
    $$PWD/Dialogs/*.h \
    $$PWD/uchardet/*.h \
    $$PWD/stdafx.h

FORMS    += $$PWD/mainwindow.ui \
    $$PWD/Dialogs/*.ui

OTHER_FILES += \
    $$PWD/ainesmile.rc \
    $$PWD/../../resource/.ainesmilerc

RESOURCES += \
    $$PWD/ainesmile.qrc

# Windows icons
RC_FILE = $$PWD/ainesmile.rc

win32: {
    INCLUDEPATH += $$(PORTED)\include
    LIBS += -L$$PWD\..\..\3rdparty\scintilla\bin -lScintillaEdit3  -L"$$(PORTED)\lib" -lIphlpapi -luser32
}

unix: !macx: {
    LIBS += -L $$PWD/../../3rdparty/scintilla/bin -lScintillaEdit
}

# Mac OS X icon
macx: {
    QMAKE_INFO_PLIST = $$PWD/osxInfo.plist
    OTHER_FILES += \
         $$PWD/osxInfo.plist
    QMAKE_CFLAGS += -gdwarf-2
    QMAKE_CXXFLAGS += -gdwarf-2
    mkdir_extensions.commands = mkdir -p \"$${TARGET}.app/Contents/PlugIns/extensions\"
    copy_themes.commands = cp -R \"$$PWD/../../resource/MacOSX/themes\" \"$${TARGET}.app/Contents/Resources\"
    copy_language.commands = cp -R \"$$PWD/../../resource/language\" \"$${TARGET}.app/Contents/Resources\"
    copy_langmap.commands = cp \"$$PWD/../../resource/langmap.xml\" \"$${TARGET}.app/Contents/Resources/\"
    copy_rc.commands = cp \"$$PWD/../../resource/.ainesmilerc\" \"$${TARGET}.app/Contents/Resources/\"
    mkdir_framework.commands = mkdir -p \"$${TARGET}.app/Contents/Frameworks\"
    clean_scintilla.depends = mkdir_framework
    clean_scintilla.commands = rm -rf \"$${TARGET}.app/Contents/Frameworks/ScintillaEdit.framework\"
    copy_scintilla.depends = clean_scintilla
    copy_scintilla.commands = cp -R \"$$PWD/../../3rdparty/scintilla/bin/ScintillaEdit.framework\" \"$${TARGET}.app/Contents/Frameworks\"

    QMAKE_EXTRA_TARGETS +=  mkdir_framework clean_scintilla copy_scintilla copy_themes copy_language copy_langmap copy_rc

    # this should be very temporary:
    MENU_NIB = $$(MENU_NIB_FILE)
    isEmpty(MENU_NIB): MENU_NIB = "FATAT_SET_MENU_NIB_FILE_ENV"
    copy_menu_nib_installer.commands = cp -R \"$$MENU_NIB\" \"$${INSTALLER_NAME}.app/Contents/Resources\"

    codesign_installer.commands = codesign -s \"$(SIGNING_IDENTITY)\" $(SIGNING_FLAGS) \"$${INSTALLER_NAME}.app\"
    dmg_installer.commands = hdiutil create -srcfolder "$${INSTALLER_NAME}.app" -volname \"Qt Creator\" -format UDBZ "ainesmile-$${PATTERN}-installer.dmg" -ov -scrub -stretch 2g
    QMAKE_EXTRA_TARGETS += codesign_installer dmg_installer copy_menu_nib_installer

    LIBS += -F $$PWD/../../3rdparty/scintilla/bin -framework ScintillaEdit \
        -framework CoreFoundation \
        -framework IOKit

    ICON = $$PWD/rc/ainesmile.icns
    icon.files += $$PWD/rc/ainesmile.png
    INSTALLS = target \
        desktop \
        icon
}

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = translations/ainesmile_en_US.ts \
                  translations/ainesmile_zh_CN.ts
