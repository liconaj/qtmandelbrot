add_library(mandelbrot_core
    STATIC
        types.h
        algorithms.h
        algorithms.cpp
)

target_include_directories(mandelbrot_core
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
