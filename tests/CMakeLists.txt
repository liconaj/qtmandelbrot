find_package(Qt6 REQUIRED COMPONENTS Test)

# mb_add_test(<name> [LIBRARIES <lib>...])
#
# Adds the Qt Test executable tst_<name>, built from tst_<name>.cpp in the
# calling directory, and registers it with CTest. The test is labeled with the
# name of that directory, which is the layer under test (core, engine, view...).
function(mb_add_test name)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "" "LIBRARIES")
    if(arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "mb_add_test: unknown arguments: ${arg_UNPARSED_ARGUMENTS}")
    endif()

    set(target tst_${name})
    get_filename_component(layer "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

    qt_add_executable(${target} ${target}.cpp)
    target_link_libraries(${target} PRIVATE Qt::Test ${arg_LIBRARIES})

    add_test(NAME ${target} COMMAND ${target})
    set_tests_properties(${target} PROPERTIES LABELS ${layer})
endfunction()

mb_add_test(sanity)

add_subdirectory(core)
