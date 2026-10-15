qt_add_executable(tst_escapetime tst_escapetime.cpp)
add_test(NAME tst_escapetime COMMAND tst_escapetime)
target_link_libraries(tst_escapetime PRIVATE Qt::Test mandelbrot_core)
