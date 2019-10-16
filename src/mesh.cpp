//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 10/1/19.
//

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <cassert>
#include "mesh.hpp"

namespace rst
{

Mesh::Mesh(const std::vector<Vertex> &vertices, std::vector<Mesh::uint> indices):
    vertices{vertices},
    indices{std::move(indices)}
{

}

inline void TransformIndex(int &index, int size) noexcept
{
    assert(index != 0);

    if (index > 0)
    {
        index -= 1;
    }
    else
    {
        index = size + index;
    }
}

Mesh Mesh::LoadFromObj(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        throw std::runtime_error("Failed to open " + filename);
    }

    std::vector<Vertex> meshVertices;
    std::vector<uint> meshIndices;
    
    std::vector<Vec3f> positions;
    std::vector<Vec2f> texCoords;
    std::vector<Vec3f> normals;

    std::string line;
    while (std::getline(in, line))
    {
        std::istringstream iss{line};
        std::string word;
        iss >> word;

        if (word == "v")
        {
            Vec3f pos;
            if (!(iss >> pos.x >> pos.y >> pos.z))
            {
                throw std::runtime_error("Obj parser: failed to parse line " + line);
            }
            positions.push_back(pos);
        }
        else if (word == "vt")
        {
            Vec2f uv;
            if (!(iss >> uv.x >> uv.y))
            {
                throw std::runtime_error("Obj parser: failed to parse line " + line);
            }
            texCoords.push_back(uv);
        }
        else if (word == "vn")
        {
            Vec3f normal;
            if (!(iss >> normal.x >> normal.y >> normal.z))
            {
                throw std::runtime_error("Obj parser: failed to parse line " + line);
            }
            normals.push_back(Normalize(normal));
        }
        else if (word == "f")
        {
            std::size_t vertSize = meshVertices.size();
            std::string face;
            while ((iss >> face))
            {
                Vertex vertex;
                int vertIndex = 0;
                int uvIndex = 0;
                int normIndex = 0;

                if (std::sscanf(face.c_str(), "%d/%d/%d", &vertIndex, &uvIndex, &normIndex) == 3)
                {
                    TransformIndex(vertIndex, positions.size());
                    vertex.pos = positions[vertIndex];
                    TransformIndex(uvIndex, texCoords.size());
                    vertex.tex = texCoords[uvIndex];
                    TransformIndex(normIndex, normals.size());
                    vertex.norm = normals[normIndex];
                }
                else if (std::sscanf(face.c_str(), "%d//%d", &vertIndex, &normIndex) == 2)
                {
                    TransformIndex(vertIndex, positions.size());
                    vertex.pos = positions[vertIndex];
                    TransformIndex(normIndex, normals.size());
                    vertex.norm = normals[normIndex];
                }
                else if (std::sscanf(face.c_str(), "%d/%d", &vertIndex, &uvIndex) == 2)
                {
                    TransformIndex(vertIndex, positions.size());
                    vertex.pos = positions[vertIndex];
                    TransformIndex(uvIndex, texCoords.size());
                    vertex.tex = texCoords[uvIndex];
                }
                else
                {
                    throw std::runtime_error("Obj parser: failed to parse line " + line);
                }

                vertex.color.r = vertex.color.g = vertex.color.b = 1;
                meshVertices.push_back(vertex);
            }

            std::size_t verticesInFace = meshVertices.size() - vertSize;
            if (verticesInFace == 0)
            {
                throw std::runtime_error("Obj parser: failed to parse line " + line);
            }

            for (std::size_t i = 1; i < verticesInFace - 1; i++)
            {
                meshIndices.push_back(vertSize);
                meshIndices.push_back(vertSize + i);
                meshIndices.push_back(vertSize + i + 1);
            }
        }
    }

    return Mesh{meshVertices, meshIndices};
}

}
