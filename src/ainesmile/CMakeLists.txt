cmake_minimum_required(VERSION 3.21)
project(ainesmile VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml Network PrintSupport LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Xml Network PrintSupport LinguistTools)
IF (Qt6_FOUND)
        find_package(Qt6 COMPONENTS Core5Compat REQUIRED)
ENDIF(Qt6_FOUND)

find_package(ICU 70.0 COMPONENTS uc i18n REQUIRED)
find_package(OpenMP)
find_package(TBB REQUIRED)
find_package(Boost 1.79.0 REQUIRED)
find_package(hwy CONFIG REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) # if you have any .ui files
set(CMAKE_AUTORCC OFF) # if you have any .qrc files

add_compile_definitions(SCINTILLA_QT=1
        SCI_LEXER=1
        _CRT_SECURE_NO_DEPRECATE=1
        SCI_STATIC_LINK=1
        NO_DLL=1
        EXPORT_IMPORT_API=
        HWY_COMPILE_ALL_ATTAINABLE=1
        )

IF(APPLE)
    add_compile_definitions(LUA_USE_MACOSX)
ENDIF(APPLE)

include_directories(${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        CodeEdit
        Dialogs
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

qt_add_resources(qrcs ainesmile.qrc)

set(TS_FILES
        translations/ainesmile_zh_CN.ts
        translations/ainesmile_zh_TW.ts
        translations/ainesmile_zh_HK.ts
        translations/ainesmile_en_US.ts
        )

IF(APPLE)
        set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/ainesmile.app/Contents/Resources/translations")
        list(APPEND ainesmile_ui ainesmile.icns)
        file(GLOB resource ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/*.xml)
ELSE(APPLE)
        set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/translations")
ENDIF(APPLE)

IF(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_create_translation(QM_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${TS_FILES}
            OPTIONS -no-obsolete -no-recursive
        )
        qt_add_executable(${PROJECT_NAME}
                ${ainesmile_headers}
                ${ainesmile_src}
                ${ainesmile_ui}
                ${qrcs}
                ${QM_FILES}
                ainesmile.rc
                )
ELSE()
        qt5_create_translation(QM_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${TS_FILES}
            OPTIONS -no-obsolete -no-recursive
        )
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
        ICU::uc ICU::i18n
        OpenMP::OpenMP_CXX
        TBB::tbb
        hwy::hwy
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Xml
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::PrintSupport
        )

IF (Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME}  PRIVATE
            Qt${QT_VERSION_MAJOR}::Core5Compat
            )
ENDIF(Qt6_FOUND)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON
                                                 MACOSX_BUNDLE ON)

if(Boost_FOUND)
  target_link_directories(${PROJECT_NAME} PUBLIC Boost::headers)
endif()

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE Shell32)
endif()

if(APPLE)
  set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/ainesmile.icns"
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources/ainesmile.icns)
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES RESOURCE "./ainesmile.icns"
               MACOSX_BUNDLE_BUNDLE_NAME "ainesmile"
               MACOSX_BUNDLE_GUI_IDENTIFIER "com.dfordsoft.ainesmile"
               MACOSX_BUNDLE_ICON_FILE "ainesmile.icns"
               MACOSX_BUNDLE_INFO_STRING "ainesmile"
               MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/osxInfo.plist"
               )
  add_custom_command(TARGET ${PROJECT_NAME} 
        POST_BUILD COMMAND 
        ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@executable_path/../Libs/"
        $<TARGET_FILE:${PROJECT_NAME}>)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/themes
    ${CMAKE_CURRENT_BINARY_DIR}/ainesmile.app/Contents/Resources/themes
    )
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/language
    ${CMAKE_CURRENT_BINARY_DIR}/ainesmile.app/Contents/Resources/language
    )
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../resource/langmap.xml" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ainesmile.app/Contents/Resources)
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../resource/.ainesmilerc" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ainesmile.app/Contents/Resources)
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/themes
    ${CMAKE_CURRENT_BINARY_DIR}/resource/themes
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../../resource/language
    ${CMAKE_CURRENT_BINARY_DIR}/resource/language
    )
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../resource/langmap.xml" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resource)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../resource/.ainesmilerc" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resource)
    target_precompile_headers(${PROJECT_NAME} PRIVATE stdafx.h)
endif()

install(
  TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
