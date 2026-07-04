cmake_minimum_required(VERSION 3.16)

project(MandelbrotSetExplorer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appqtmandelbrot
    main.cpp


)

qt_add_qml_module(appqtmandelbrot
    URI com.liconaj.qtmandelbrot
    QML_FILES
        Main.qml
    SOURCES
        mandelbrotrenderer.h
        mandelbrotrenderer.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appqtmandelbrot PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appqtmandelbrot
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appqtmandelbrot
    PRIVATE
        Qt6::Quick
        Qt6::Concurrent
)

install(TARGETS appqtmandelbrot
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
