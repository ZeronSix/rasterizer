//
// Created by zhdanovskiy on 16.10.2019.
//

#ifndef BASIC_FRAGMENT_SHADER_HPP
#define BASIC_FRAGMENT_SHADER_HPP

#include "basic_vertex_shader.hpp"

namespace rst
{

struct BasicFragmentShader
{
    using InType = BasicVertexShader::OutType;

    Vec4f operator()(const InType &in)
    {
        return Vec4f{1.0f, 1.0f, 1.0f, 0.0f};
    }
};

}

#endif //BASIC_FRAGMENT_SHADER_HPP
