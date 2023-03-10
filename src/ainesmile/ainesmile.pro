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
include($$PWD/../../3rdparty/scintilla-latest.pri)
include($$PWD/../../3rdparty/lexilla-latest.pri)

PRECOMPILED_HEADER = stdafx.h

INCLUDEPATH += $$PWD/CodeEdit \
    $$PWD/Dialogs \
    $$PWD/uchardet 

DEFINES += LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT

SOURCES += $$PWD/main.cpp\
    $$PWD/mainwindow.cpp \
    $$PWD/tabwidget.cpp \
    $$PWD/config.cpp \
    $$files(CodeEdit/*.cpp, false) \
    $$files(Dialogs/*.cpp, false)  \
    $$files(uchardet/*.cpp, false)

HEADERS  += $$PWD/mainwindow.h \
    $$PWD/tabwidget.h \
    $$PWD/config.h \
    $$files(CodeEdit/*.h, false)  \
    $$files(Dialogs/*.h, false)  \
    $$files(uchardet/*.h, false)  \
    $$PWD/stdafx.h

FORMS    += $$PWD/mainwindow.ui \
    $$files(Dialogs/*.ui, false)

OTHER_FILES += \
    $$PWD/ainesmile.rc \
    $$PWD/../../resource/.ainesmilerc

RESOURCES += \
    $$PWD/ainesmile.qrc

win32: {
    SOURCES += ShellContextMenu.cpp
    HEADERS += ShellContextMenu.h
    # Windows icons
    RC_FILE = $$PWD/ainesmile.rc
    LIBS += -lUser32
    WINDEPLOYQT = $$[QT_INSTALL_BINS]/windeployqt.exe
    CONFIG(release, debug|release): {
        QMAKE_POST_LINK += $$quote($$WINDEPLOYQT --release --force \"$${OUT_PWD}/Release/$${TARGET}.exe\")
    } else: {
        QMAKE_POST_LINK += $$quote($$WINDEPLOYQT --force \"$${OUT_PWD}/Debug/$${TARGET}.exe\")
    }
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

    APPCERT = Developer ID Application: Fan Yang (Y73SBCN2CG)
    INSTALLERCERT = 3rd Party Mac Developer Installer: Fan Yang (Y73SBCN2CG)

    BUNDLEID = com.dfordsoft.ainesmile
    ENTITLEMENTS = $$PWD/ainesmile/ainesmile.entitlements

    OTHER_FILES += $$PWD/osxInfo.plist $${ENTITLEMENTS}

    codesign.commands += dsymutil $${TARGET}.app/Contents/MacOS/$${TARGET} -o $${TARGET}.app.dSYM;

    mkdir_extensions.commands = mkdir -p \"$${TARGET}.app/Contents/PlugIns/extensions\"
    copy_themes.commands = cp -R \"$$PWD/../../resource/themes\" \"$${TARGET}.app/Contents/Resources\"
    copy_language.commands = cp -R \"$$PWD/../../resource/language\" \"$${TARGET}.app/Contents/Resources\"
    copy_langmap.commands = cp \"$$PWD/../../resource/langmap.xml\" \"$${TARGET}.app/Contents/Resources/\"
    copy_rc.commands = cp \"$$PWD/../../resource/.ainesmilerc\" \"$${TARGET}.app/Contents/Resources/\"

    MACDEPLOYQT = $$[QT_INSTALL_BINS]/macdeployqt
    deploy.depends += copy_themes copy_language copy_langmap copy_rc
    deploy.commands += $$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\" -appstore-compliant
    codesign_bundle.depends += deploy
    codesign_bundle.commands = codesign -s \"$${APPCERT}\" -v --timestamp=none --entitlements \"$${ENTITLEMENTS}\" -f --deep \"$${OUT_PWD}/$${TARGET}.app\"
    makedmg.depends += codesign_bundle
    makedmg.commands = hdiutil create -srcfolder "ainesmile.app" -volname \"ainesmile\" -format UDBZ "ainesmile-installer.dmg" -ov -scrub -stretch 2g
    QMAKE_EXTRA_TARGETS +=  copy_themes copy_language copy_langmap copy_rc deploy codesign_bundle makedmg

    LIBS += -framework CoreFoundation \
        -framework IOKit

    ICON = $$PWD/rc/ainesmile.icns
}

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = translations/ainesmile_en_US.ts \
                  translations/ainesmile_zh_CN.ts
