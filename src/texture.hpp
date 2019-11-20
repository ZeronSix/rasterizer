//
// Created by Vyacheslav Zhdanovskiy <zeronsix@gmail.com> on 11/20/19.
//

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <fstream>
#include <string>
#include <assert.h>
#include "math.hpp"
#include "aligning_mallocator.hpp"

namespace rst
{

class Texture
{
public:
    explicit Texture(const std::string &filename);
    Vec4f    Fetch(const Vec2f &uv);
public:
    //AlignedVec<Vec4f, sizeof(Vec4f)> m_buf;
    std::vector<Vec4f> m_buf;
    float m_width;
    float m_height;
};

Texture::Texture(const std::string &filename):
    m_width{0}, m_height{0}
{
    std::ifstream in{filename};
    if (!in.is_open()) throw std::runtime_error("Failed to open " + filename);

    std::string s;
    if (!(in >> s) || s != "P6") throw std::runtime_error("Failed to parse the magic number");

    std::size_t width{0}, height{0};
    if (!(in >> width >> height)) throw std::runtime_error("Failed to parse the image size");
    m_width = width;
    m_height = height;

    m_buf.reserve(width * height);

    unsigned maxColorValue = 0;
    if (!(in >> maxColorValue) || maxColorValue > 255) throw std::runtime_error("Failed to parse the max color value");

    in.unsetf(std::ios_base::skipws);
    in.ignore(1);

    for (auto y = 0ul; y < height; ++y)
    {
        for (auto x = 0ul; x < width; ++x)
        {
            struct { unsigned char r; unsigned char g; unsigned char b; } pixel{};

            if (!(in.read(reinterpret_cast<char *>(&pixel), 3)))
            {
                std::cerr << x << y << "\n";
                throw std::runtime_error("Failed to parse pixels");
            }
            m_buf[width * y + x] = Vec4f(pixel.b, pixel.g, pixel.r, maxColorValue) / maxColorValue;
        }
    }
}

inline Vec4f Texture::Fetch(const Vec2f &uv)
{
    std::size_t x = std::lround(uv.x * m_width);
    std::size_t y = std::lround((1 - uv.y) * m_height);
    return m_buf[m_width * y + x];
}

}

#endif //TEXTURE_HPP
