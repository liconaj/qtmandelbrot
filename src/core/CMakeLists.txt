add_library(mandelbrot_core
    STATIC
        numeric.h
        escapetime.h
        escapetime.cpp
        viewport.h
        viewport.cpp
)

target_include_directories(mandelbrot_core
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
