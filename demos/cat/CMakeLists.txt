add_executable(cat cat.cpp ${RASTERIZER_SRC})
message("${RASTERIZER_SRC}")
target_compile_options(cat PUBLIC -O3 -march=native)
target_link_options(cat PUBLIC -pthread)
set_target_properties(cat
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_CURRENT_SOURCE_DIR}")

