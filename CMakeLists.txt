cmake_minimum_required(VERSION 3.5)

project(Qt6_5_TLW_SurfaceChangeOnSetParent VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Qml QuickWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Qml QuickWidgets)

set(PROJECT_SOURCES
        main.cpp
        qwinhost.cpp
        qwinhost.h
)

qt_add_executable(Qt6_5_TLW_SurfaceChangeOnSetParent
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

qt_add_resources(Qt6_5_TLW_SurfaceChangeOnSetParent
    "qml"
    PREFIX "/"
    FILES Panel.qml
)

target_link_libraries(Qt6_5_TLW_SurfaceChangeOnSetParent PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::QuickWidgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Qt6_5_TLW_SurfaceChangeOnSetParent)
endif()
set_target_properties(Qt6_5_TLW_SurfaceChangeOnSetParent PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Qt6_5_TLW_SurfaceChangeOnSetParent
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(Qt6_5_TLW_SurfaceChangeOnSetParent)
