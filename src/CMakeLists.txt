set(SOURCES main.cpp math.hpp mesh.cpp mesh.hpp mouse.cpp mouse.hpp rasterizer.hpp screen_buffer.hpp tty_context.hpp
            tty_context.cpp shaders/basic_fragment_shader.hpp shaders/basic_vertex_shader.hpp
            shaders/phong_fragment_shader.hpp shaders/phong_vertex_shader.hpp shaders/shaders.hpp)
add_executable(rasterizer ${SOURCES})
target_compile_options(rasterizer PUBLIC -O3 -march=native)

