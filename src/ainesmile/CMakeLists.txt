cmake_minimum_required(VERSION 3.21)
project(ainesmile VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml Network  LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Xml Network  LinguistTools)
IF (Qt6_FOUND)
        find_package(Qt6 COMPONENTS Core5Compat REQUIRED)
ENDIF(Qt6_FOUND)

find_package ( uchardet )

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

if (MSVC)
    add_compile_options(/utf-8)
endif()
IF(WIN32)
    add_compile_definitions(UNICODE)
ENDIF(WIN32)

add_compile_definitions(SCINTILLA_QT=1
        SCI_LEXER=1
        _CRT_SECURE_NO_DEPRECATE=1
        SCI_STATIC_LINK=1
        NO_DLL=1
        EXPORT_IMPORT_API=
        )

IF(APPLE)
    add_compile_definitions(LUA_USE_MACOSX)
ENDIF(APPLE)

include_directories(${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        CodeEdit
        Dialogs
        uchardet
        )

file(GLOB
        CodeEdit_headers
        CodeEdit/*.h
        )
file(GLOB
        Dialogs_headers
        Dialogs/*.h
        )

set(ainesmile_headers
        stdafx.h
        tabwidget.h
        config.h
		${CodeEdit_headers}
		${Dialogs_headers}
        )

file(GLOB
        CodeEdit_src
        CodeEdit/*.cpp
        )
file(GLOB
        Dialogs_src
        Dialogs/*.cpp
        )

set(ainesmile_src
        main.cpp
        mainwindow.cpp
        tabwidget.cpp
        config.cpp
		${CodeEdit_src}
		${Dialogs_src}
        )

file(GLOB
        Dialogs_ui
        Dialogs/*.ui
        )
set(ainesmile_ui
        mainwindow.ui
		${Dialogs_ui}
        )
IF(WIN32)
    list(APPEND ainesmile_headers 
        ShellContextMenu.h
        )
    list(APPEND ainesmile_src 
        ShellContextMenu.cpp
        )
ENDIF(WIN32)        
IF(APPLE)
ENDIF(APPLE)

qt_add_resources(qrcs ainesmile.qrc)

set(TS_FILES 
        translations/ainesmile_zh_CN.ts
        translations/ainesmile_en_US.ts
        )

IF(APPLE)
        set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ainesmile.app/Contents/Resources/translations")
ELSE(APPLE)
        set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")
ENDIF(APPLE)

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        # qt_create_translation(QM_FILES 
        #     ${CMAKE_CURRENT_SOURCE_DIR} 
        #     ${TS_FILES}
        #     OPTIONS -no-obsolete
        # )
        qt_add_executable(${PROJECT_NAME}  
                ${ainesmile_headers}
                ${ainesmile_src}
                ${ainesmile_ui}
                ${qrcs}
                ${QM_FILES}
                ainesmile.rc
                )
ELSE()
        # qt5_create_translation(QM_FILES
        #     ${CMAKE_CURRENT_SOURCE_DIR}
        #     ${TS_FILES}
        #     OPTIONS -no-obsolete
        # )
        add_executable(${PROJECT_NAME}  
                ${ainesmile_headers}
                ${ainesmile_src}
                ${ainesmile_ui}
                ${qrcs}
                ${QM_FILES}
                ainesmile.rc
                )
ENDIF()

target_link_libraries(${PROJECT_NAME}  PRIVATE
        scintilla
        lexilla
        uchardet::libuchardet
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Widgets
        )
IF (Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME}  PRIVATE
            Qt${QT_VERSION_MAJOR}::Core5Compat
            )
ENDIF(Qt6_FOUND)


set_target_properties(${PROJECT_NAME}  PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        )
