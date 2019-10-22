//
// Created by zhdanovskiy on 16.10.2019.
//

#ifndef BASIC_VERTEX_SHADER_HPP
#define BASIC_VERTEX_SHADER_HPP

#include "../mesh.hpp"
#include "../math.hpp"

namespace rst
{

struct BasicVertexShader
{
    using InType = Mesh::Vertex;
    struct OutType
    {
        Vec4f pos;
        Vec3f norm;
    };

    Mat4f modelMatrix;
    Mat4f cameraMatrix;
    Mat4f normalMatrix;

    OutType operator()(const InType &in)
    {
        Vec4f inPos4 = Vec4f(in.pos, 1.0f);
        Vec4f outPos = cameraMatrix * modelMatrix * inPos4;

        OutType out{
            outPos,
            Vec3f{normalMatrix * Vec4f{in.norm, 1.0f}}
        };
        return out;
    }
};

}


#endif //BASIC_VERTEX_SHADER_HPP
