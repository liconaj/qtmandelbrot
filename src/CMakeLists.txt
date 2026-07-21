find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appqtmandelbrot
    main.cpp
)

set(QT_QML_GENERATE_QMLLS_INI ON)
qt_add_qml_module(appqtmandelbrot
    URI com.liconaj.qtmandelbrot
    QML_FILES
        Main.qml
    SOURCES
        mandelbrot.h
        mandelbrot.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appqtmandelbrot PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appqtmandelbrot
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
)

add_subdirectory(core)

target_link_libraries(appqtmandelbrot
    PRIVATE
        Qt6::Quick
        Qt6::Concurrent
        mandelbrot_core
)

install(TARGETS appqtmandelbrot
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
