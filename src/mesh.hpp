//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 10/1/19.
//

#ifndef MESH_HPP
#define MESH_HPP

#include "math.hpp"
#include "tty_context.hpp"
#include <vector>
#include <string>

namespace rst
{

struct Mesh
{
public:
    using uint = unsigned;

    struct Vertex
    {
        Vec3f pos;
        Vec2f tex;
        Vec3f norm;
        Color color;
    };

    std::vector<Vertex> vertices;
    std::vector<uint>   indices;

    Mesh(const std::vector<Vertex>& vertices, std::vector<uint> indices);
    static Mesh LoadFromObj(const std::string& filename);
};

};

#endif //MESH_HPP
