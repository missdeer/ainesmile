#-------------------------------------------------
#
# Project created by QtCreator 2013-08-05T10:00:14
#
#-------------------------------------------------

QT       -= core gui

TARGET = utility
TEMPLATE = lib
CONFIG += staticlib

VERSION = 1.0.0

include(../../Boost.pri)

SOURCES += \
    hardware.cpp \
    hardware_linux.cpp \
    hardware_osx.cpp \
    hardware_unsupported.cpp \
    hardware_win32.cpp \
    hardware_bsd.cpp \
    socket_util.cpp \
    stdafx.cpp \
    util.cpp

HEADERS += \
    hardware.hpp \
    hardware_linux.hpp \
    hardware_osx.hpp \
    hardware_unsupported.hpp \
    hardware_win32.hpp \
    hardware_bsd.hpp \
    resource.h \
    socket_util.hpp \
    stdafx.h \
    sysdef.h \
    util.hpp \
    utility_global.hpp \
    datatype.hpp

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    utility.rc

# RC_FILE = utility.rc

