cmake_minimum_required(VERSION 3.21)
project(ainesmile VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml Network WebEngine WebEngineWidgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Xml Network WebEngine WebEngineWidgets LinguistTools)
IF (Qt6_FOUND)
        find_package(Qt6 COMPONENTS Core5Compat REQUIRED)
ENDIF(Qt6_FOUND)

find_package(Boost 1.79.0)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})

    set(Boost_USE_STATIC_LIBS ON)

    set(Boost_USE_MULTITHREADED ON)
    if(MSVC)
      set(Boost_USE_STATIC_RUNTIME ON)
    endif(MSVC)
endif(Boost_FOUND)

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
        ${CMAKE_CURRENT_SOURCE_DIR}/CodeEdit
        ${CMAKE_CURRENT_SOURCE_DIR}/Dialogs
        ${CMAKE_CURRENT_SOURCE_DIR}/uchardet
        )

file(GLOB
        CodeEdit_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/CodeEdit/*.h
        )
file(GLOB
        Dialogs_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/Dialogs/*.h
        )
file(GLOB
        uchardet_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/uchardet/*.h
        )
set(ainesmile_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
        ${CMAKE_CURRENT_SOURCE_DIR}/tabwidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/config.h
		${CodeEdit_headers}
		${Dialogs_headers}
		${uchardet_headers}
        )

file(GLOB
        CodeEdit_src
        ${CMAKE_CURRENT_SOURCE_DIR}/CodeEdit/*.cpp
        )
file(GLOB
        Dialogs_src
        ${CMAKE_CURRENT_SOURCE_DIR}/Dialogs/*.cpp
        )
file(GLOB
        uchardet_src
        ${CMAKE_CURRENT_SOURCE_DIR}/uchardet/*.cpp
        )

set(ainesmile_src
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tabwidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp
		${CodeEdit_src}
		${Dialogs_src}
		${uchardet_src}
        )

file(GLOB
        Dialogs_ui
        ${CMAKE_CURRENT_SOURCE_DIR}/Dialogs/*.ui
        )
set(ainesmile_ui
        ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui 
		${Dialogs_ui}
        )
IF(WIN32)
    list(APPEND ainesmile_headers 
        ${CMAKE_CURRENT_SOURCE_DIR}/ShellContextMenu.h
        )
    list(APPEND ainesmile_src 
        ${CMAKE_CURRENT_SOURCE_DIR}/ShellContextMenu.cpp
        )
ENDIF(WIN32)        
IF(APPLE)
ENDIF(APPLE)

qt_add_resources(qrcs ${CMAKE_CURRENT_SOURCE_DIR}/ainesmile.qrc)

set(TS_FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/ainesmile_zh_CN.ts
        ${CMAKE_CURRENT_SOURCE_DIR}/translations/ainesmile_en_US.ts
        )

IF(APPLE)
        set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ainesmile.app/Contents/Resources/translations")
ELSE(APPLE)
        set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations")
ENDIF(APPLE)

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_create_translation(QM_FILES 
            ${CMAKE_CURRENT_SOURCE_DIR} 
            ${TS_FILES}
            OPTIONS -no-obsolete
        )
        qt_add_executable(${PROJECT_NAME}  
                ${ainesmile_headers}
                ${ainesmile_src}
                ${ainesmile_ui}
                ${qrcs}
                ${QM_FILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/ainesmile.rc
                )
ELSE()
#        qt5_create_translation(QM_FILES
#            ${CMAKE_CURRENT_SOURCE_DIR}
#            ${TS_FILES}
#        )
        add_executable(${PROJECT_NAME}  
                ${ainesmile_headers}
                ${ainesmile_src}
                ${ainesmile_ui}
                ${qrcs}
                ${QM_FILES}
                ${CMAKE_CURRENT_SOURCE_DIR}/ainesmile.rc
                )
ENDIF()

target_link_libraries(${PROJECT_NAME}  PRIVATE
        scintilla
        lexilla
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::WebEngine
        Qt${QT_VERSION_MAJOR}::WebEngineWidgets
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
