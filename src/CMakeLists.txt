set(SOURCES main.cpp math.hpp mesh.cpp mesh.hpp mouse.cpp mouse.hpp rasterizer.hpp screen_buffer.hpp tty_context.hpp tty_context.cpp)
add_executable(rasterizer ${SOURCES})
target_compile_options(rasterizer PUBLIC -O3 -march=native)

