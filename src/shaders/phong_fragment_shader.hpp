//
// Created by zhdanovskiy on 16.10.2019.
//

#ifndef PHONG_FRAGMENT_SHADER_HPP
#define PHONG_FRAGMENT_SHADER_HPP

#include "phong_vertex_shader.hpp"

namespace rst
{

struct PhongFragmentShader
{
    using InType = PhongVertexShader::OutType;

    Vec3f lightWorldPos{0, 0, 0};
    Vec4f lightColor{1, 1, 1, 1};
    float specularCoeff = 1.0f;
    float specularPower = 100;
    float diffuseCoeff = 90.0f;
    float ambientIntensity = 0.2f;
    float tonemappingCoeff = 0.25f;

    Vec4f operator()(const InType &in)
    {
        Vec3f dir = lightWorldPos - in.worldPos;
        Vec3f h = Normalize(Normalize(dir) - Normalize(in.worldPos));

        float diffuseIntensity = std::max(0.0f, Dot(in.norm, Normalize(dir)));
        diffuseIntensity /= SqrMagnitude(dir);
        float specularIntensity = std::pow(std::max(Dot(h, in.norm), 0.0f), specularPower);
        float intensity = ambientIntensity + diffuseCoeff * diffuseIntensity + specularCoeff * specularIntensity;
        float mappedIntensity = 1 - std::exp(-intensity * tonemappingCoeff);
        return mappedIntensity * lightColor;
    }
};

}

#endif //PHONG_FRAGMENT_SHADER_HPP
