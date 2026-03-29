cmake_minimum_required(VERSION 3.16)

project(ogr2gui VERSION 0.6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)
find_package(GDAL REQUIRED)

set(OGR2GUI_SOURCES
    src/App.cpp
    src/Inf.cpp
    src/Ogr.cpp
)

set(OGR2GUI_HEADERS
    config.h
    inc/App.h
    inc/Dta.h
    inc/Inf.h
    inc/Ogr.h
)

set(OGR2GUI_TRANSLATIONS
    ogr2gui_zh.ts
    ogr2gui_zh.qm
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(ogr2gui
        MANUAL_FINALIZATION
        MACOSX_BUNDLE
        WIN32
        ${OGR2GUI_SOURCES}
        ${OGR2GUI_HEADERS}
        ${OGR2GUI_TRANSLATIONS}
    )
else()
    add_executable(ogr2gui
        MACOSX_BUNDLE
        WIN32
        ${OGR2GUI_SOURCES}
        ${OGR2GUI_HEADERS}
        ${OGR2GUI_TRANSLATIONS}
    )
endif()

target_include_directories(ogr2gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(ogr2gui PRIVATE
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Widgets
    GDAL::GDAL
)

add_custom_command(TARGET ogr2gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/ogr2gui_zh.qm
            $<TARGET_FILE_DIR:ogr2gui>/ogr2gui_zh.qm
)

install(TARGETS ogr2gui
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ogr2gui_zh.qm
    DESTINATION bin
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_finalize_executable(ogr2gui)
endif()
