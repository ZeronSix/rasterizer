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
        Vec3f pos;
        Vec3f norm;
    };

    Mat4f modelMatrix;
    Mat4f cameraMatrix;
    Mat3f normalMatrix;

    OutType operator()(const InType &in)
    {
        Vec4f inPos4 = Vec4f(in.pos, 1.0f);
        Vec4f outPos = cameraMatrix * inPos4;
        outPos = outPos.w / 4;

        OutType out{
            Vec3f{inPos4},
            Vec3f{normalMatrix * in.norm}
        };
        return out;
    }
};

}


#endif //BASIC_VERTEX_SHADER_HPP
