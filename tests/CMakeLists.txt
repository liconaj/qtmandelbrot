find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(tst_sanity
    core/tst_sanity.cpp
)

add_test(NAME tst_sanity COMMAND tst_sanity)

target_link_libraries(tst_sanity PRIVATE Qt::Test)
