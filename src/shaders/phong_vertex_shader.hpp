//
// Created by zhdanovskiy on 16.10.2019.
//

#ifndef PHONG_VERTEX_SHADER_HPP
#define PHONG_VERTEX_SHADER_HPP

#include "../mesh.hpp"
#include "../math.hpp"

namespace rst
{

struct PhongVertexShader
{
    using InType = Mesh::Vertex;
    struct OutType
    {
        Vec4f pos;
        Vec3f norm;
        Vec3f worldPos;
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
            Normalize(Vec3f{normalMatrix * Vec4f{in.norm, 1.0f}}),
            Vec3f(modelMatrix * inPos4)
        };
        return out;
    }
};

}


#endif //PHONG_VERTEX_SHADER_HPP
