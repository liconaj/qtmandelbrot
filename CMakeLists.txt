cmake_minimum_required(VERSION 3.16)

project(MandelbrotSetExplorer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Concurrent)

qt_standard_project_setup(REQUIRES 6.10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")

set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(src)

# Enable the tests when BUILD_TESTING is ON.
option(BUILD_TESTING "Build the tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

add_custom_target(copy_compile_commands ALL
    COMMAND
        "${CMAKE_COMMAND}" -E copy_if_different
        "${PROJECT_BINARY_DIR}/compile_commands.json"
        "${PROJECT_SOURCE_DIR}/compile_commands.json"
    VERBATIM
)
