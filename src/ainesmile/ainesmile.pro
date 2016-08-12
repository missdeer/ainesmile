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

win32: {
    # Windows icons
    RC_FILE = $$PWD/ainesmile.rc
    LIBS += -lUser32
}

# Mac OS X icon
macx: {
    QMAKE_INFO_PLIST = $$PWD/osxInfo.plist

    QMAKE_CFLAGS += -gdwarf-2
    QMAKE_CXXFLAGS += -gdwarf-2
    QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_OBJECTIVE_CFLAGS_RELEASE =  $$QMAKE_OBJECTIVE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
    # Name of the application signing certificate
    APPCERT = "3rd Party Mac Developer Application: Fan Yang"
    # Name of the installer signing certificate
    INSTALLERCERT = "3rd Party Mac Developer Installer: Fan Yang"
    # Bundle identifier for your application
    BUNDLEID = com.dfordsoft.ainesmile
    # Name of the entitlements file (only needed if you want to sandbox the application)
    ENTITLEMENTS = $$PWD/ainesmile/ainesmile.entitlements

    OTHER_FILES += $$PWD/osxInfo.plist $${ENTITLEMENTS}

    codesign.depends  += all
    codesign.commands += macdeployqt $${TARGET}.app;
    # Extract debug symbols
    codesign.commands += dsymutil $${TARGET}.app/Contents/MacOS/$${TARGET} -o $${TARGET}.app.dSYM;
    # Sign the application bundle, using the provided entitlements
    codesign.commands += codesign -f -s $${APPCERT} -v –entitlements $${ENTITLEMENTS} $${TARGET}.app;
    product.depends += all
    # Build the product package
    product.commands += productbuild –component $${TARGET}.app /Applications –sign $${INSTALLERCERT} $${TARGET}.pkg;

    mkdir_extensions.commands = mkdir -p \"$${TARGET}.app/Contents/PlugIns/extensions\"
    copy_themes.commands = cp -R \"$$PWD/../../resource/themes\" \"$${TARGET}.app/Contents/Resources\"
    copy_language.commands = cp -R \"$$PWD/../../resource/language\" \"$${TARGET}.app/Contents/Resources\"
    copy_langmap.commands = cp \"$$PWD/../../resource/langmap.xml\" \"$${TARGET}.app/Contents/Resources/\"
    copy_rc.commands = cp \"$$PWD/../../resource/.ainesmilerc\" \"$${TARGET}.app/Contents/Resources/\"

    MACDEPLOYQT = $$[QT_INSTALL_BINS]/macdeployqt
    deploy.depends =  copy_themes copy_language copy_langmap copy_rc
    deploy.commands = $$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\"  -appstore-compliant
    QMAKE_EXTRA_TARGETS += copy_themes copy_language copy_langmap copy_rc deploy

    POST_TARGETDEPS += copy_themes copy_language copy_langmap copy_rc
    QMAKE_POST_LINK += $$quote($$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\" -appstore-compliant)

    codesign_installer.commands = codesign -s \"$(SIGNING_IDENTITY)\" $(SIGNING_FLAGS) \"$${INSTALLER_NAME}.app\"
    dmg_installer.commands = hdiutil create -srcfolder "$${INSTALLER_NAME}.app" -volname \"Qt Creator\" -format UDBZ "ainesmile-$${PATTERN}-installer.dmg" -ov -scrub -stretch 2g
    QMAKE_EXTRA_TARGETS += codesign_installer dmg_installer

    LIBS += -framework CoreFoundation \
        -framework IOKit

    ICON = $$PWD/rc/ainesmile.icns
}

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = translations/ainesmile_en_US.ts \
                  translations/ainesmile_zh_CN.ts
