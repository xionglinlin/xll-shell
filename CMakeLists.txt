cmake_minimum_required(VERSION 3.5)

project(xll-shell VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Quick)
find_package(Dtk${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

message(STATUS "${CMAKE_INSTALL_LIBDIR}")

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        pluginloader.cpp
        pluginloader.h
        pluginmetadata.cpp
        pluginmetadata.h
        dsglobal.h
        pluginfactory.cpp
        pluginfactory.h
        applet.cpp
        applet.h
        appletdata.cpp
        appletdata.h
        appletitem.cpp
        appletitem.h
        appletitemmodel.cpp
        appletitemmodel.h
        containment.cpp
        containment.h
        containmentitem.cpp
        containmentitem.h
        panel.cpp
        panel.h
        qmlengine.cpp
        qmlengine.h
        dsqmlglobal.cpp
        appletloader.cpp
        appletloader.h
)

set(PRIVATE_HEADERS
    private/applet_p.h
    private/containment_p.h
    private/panel_p.h
    private/appletitem_p.h
    private/dsqmlglobal_p.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(xll-shell
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PRIVATE_HEADERS}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET xll-shell APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(xll-shell SHARED
            ${PROJECT_SOURCES}
            ${PRIVATE_HEADERS}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(xll-shell
            ${PROJECT_SOURCES}
            ${PRIVATE_HEADERS}
        )
    endif()
endif()

target_include_directories(xll-shell PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)

target_link_directories(xll-shell PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${LIB_INSTALL_DIR}>
)

target_link_libraries(xll-shell PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Quick
    Dtk${QT_VERSION_MAJOR}::Core
    Dtk${QT_VERSION_MAJOR}::Gui
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.xll-shell)
endif()
set_target_properties(xll-shell PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS xll-shell
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(xll-shell)
endif()
