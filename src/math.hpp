//
// Created by Vyacheslav Zhdanovskiy <zhdanovskiy@iitp.ru> on 9/30/19.
//

#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>
#include <cstddef>

namespace rst
{

constexpr float DEG2RAD{M_PI / 180.0f};
constexpr float RAD2DEG{1.0f / DEG2RAD};

template<typename T, std::size_t N>
union Vec
{
    T data[N];

    T       &operator[](int i)       noexcept { return data[i]; }
    const T &operator[](int i) const noexcept { return data[i]; }
};

template<typename T, std::size_t N>
Vec<T, N> operator+(const Vec<T, N> &lhs, const Vec<T, N> &rhs) noexcept
{
    Vec<T, N> result{};

    for (std::size_t i = 0; i < N; ++i)
    {
        result[i] = lhs.data[i] + rhs.data[i];
    }

    return result;
}

template<typename T, std::size_t N>
Vec<T, N> operator-(const Vec<T, N> &lhs, const Vec<T, N> &rhs) noexcept
{
    Vec<T, N> result{};

    for (std::size_t i = 0; i < N; ++i)
    {
        result[i] = lhs.data[i] - rhs.data[i];
    }

    return result;
}

template<typename T, std::size_t N>
Vec<T, N> operator*(const Vec<T, N> &lhs, const Vec<T, N> &rhs) noexcept
{
    Vec<T, N> result{};

    for (std::size_t i = 0; i < N; ++i)
    {
        result[i] = lhs.data[i] * rhs.data[i];
    }

    return result;
}

template<typename T, std::size_t N>
Vec<T, N> operator*(float lhs, const Vec<T, N> &rhs) noexcept
{
    Vec<T, N> result{};

    for (std::size_t i = 0; i < N; ++i)
    {
        result[i] = lhs * rhs.data[i];
    }

    return result;
}

template<typename T, std::size_t N>
Vec<T, N> operator/(const Vec<T, N> &lhs, float rhs) noexcept
{
    Vec<T, N> result{};

    for (std::size_t i = 0; i < N; ++i)
    {
        result[i] = lhs.data[i] / rhs;
    }

    return result;
}

template<typename T, std::size_t N>
float Dot(const Vec<T, N> &lhs, const Vec<T, N> &rhs) noexcept
{
    float result{0.0f};

    for (std::size_t i = 0; i < N; ++i)
    {
        result += lhs.data[i] * rhs.data[i];
    }

    return result;
}

template<typename T, std::size_t N>
float SqrMagnitude(const Vec<T, N> &vec) noexcept { return Dot(vec, vec); }

template<typename T, std::size_t N>
float Magnitude(const Vec<T, N> &vec) noexcept { return std::sqrt(SqrMagnitude(vec)); }

template<typename T, std::size_t N>
Vec<T, N> Normalize(const Vec<T, N> &vec) noexcept
{
    float magnitude = Magnitude(vec);
    Vec<T, N> result{};

    for (std::size_t i = 0; i < N; i++)
    {
        result[i] = vec[i] / magnitude;
    }

    return result;
}

template<typename T>
union Vec<T, 2>
{
    struct
    {
        float x;
        float y;
    };
    T data[2];

    T       &operator[](int i)       noexcept { return data[i]; }
    const T &operator[](int i) const noexcept { return data[i]; }
};

template<typename T>
union Vec<T, 3>
{
    struct
    {
        float x;
        float y;
        float z;
    };
    T data[3];

    Vec<T, 3>() = default;
    Vec<T, 3>(float a, float b, float c) noexcept:
        x{a}, y{b}, z{c} {}
    explicit Vec<T, 3>(const Vec<T, 4>& vec) noexcept:
        x{vec.x / vec.w},
        y{vec.y / vec.w},
        z{vec.z / vec.w} {}

    T       &operator[](int i)       noexcept { return data[i]; }
    const T &operator[](int i) const noexcept { return data[i]; }
};

template<typename T>
Vec<T, 3> Cross(const Vec<T, 3> &lhs, const Vec<T, 3> &rhs) noexcept
{
    return Vec<T, 3>{lhs.y * rhs.z - lhs.z * rhs.y,
                     lhs.z * rhs.x - lhs.x * rhs.z,
                     lhs.x * rhs.y - lhs.y * rhs.x};
}

template<typename T>
union Vec<T, 4>
{
    struct
    {
        T x;
        T y;
        T z;
        T w;
    };
    T data[4];

    Vec(T a = 0, T b = 0, T c = 0, T d = 0) noexcept:
        x{a}, y{b}, z{c}, w{d} {}
    explicit Vec(const Vec<T, 3> vec3, T w_) noexcept:
        x{vec3.x},
        y{vec3.y},
        z{vec3.z},
        w{w_} {}

    T       &operator[](int i)       noexcept { return data[i]; }
    const T &operator[](int i) const noexcept { return data[i]; }
};

using Vec2i = Vec<int, 2>;
using Vec3i = Vec<int, 3>;
using Vec4i = Vec<int, 4>;
using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;

template<typename T, std::size_t N>
struct Mat
{
    T data[N][N];

    // Mat() noexcept : data{} {}

    T       *operator[](int i)       noexcept { return data[i]; }
    const T *operator[](int i) const noexcept { return data[i]; }

    static constexpr Mat<T, N> Id() noexcept
    {
        Mat<T, N> result{};
        for (std::size_t i = 0; i < N; i++)
        {
            result[i][i] = 1;
        }
    }
};

template<typename T, std::size_t N>
Vec<T, N> operator*(const Mat<T, N> &lhs, const Vec<T, N> &rhs) noexcept
{
    Vec<T, N> result{};
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            result[i] += lhs[i][j] * rhs[j];
        }
    }

    return result;
}

template<typename T, std::size_t N>
Mat<T, N> operator*(const Mat<T, N> &lhs, const Mat<T, N> &rhs) noexcept
{
    Mat<T, N> result{};
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                result[i][j] += lhs[i][k] * rhs[k][j];
            }
        }
    }

    return result;
}

using Mat3f = Mat<float, 3>;
using Mat4f = Mat<float, 4>;

template<typename T>
T Clamp(T value, T min, T max) noexcept
{
    return std::min(min, std::max(value, max));
}

inline Mat3f LookRotation(const Vec3f &dir, const Vec3f &up) noexcept
{
    Vec3f right   = Normalize(Cross(up, dir));
    Vec3f newUp   = Normalize(Cross(dir, right));
    Vec3f normDir = Normalize(dir);

    return Mat3f{
        right.x, right.y, right.z,
        newUp.x, newUp.y, newUp.z,
        normDir.x, normDir.y, normDir.z
    };
}

inline Mat4f ProjectionMatrix(float horizontalFov, float aspectRatio, float nearClipPlane, float farClipPlane)
{
    float cotHalfHorizontalFov = 1.0f / std::tan(horizontalFov / 2);
    float cotHalfVerticalFov   = 1.0f / std::tan(horizontalFov / 2) * aspectRatio;
    float depthCoeff1          = -(farClipPlane + nearClipPlane) / (farClipPlane - nearClipPlane);
    float depthCoeff2          = -2.0f * farClipPlane * nearClipPlane / (farClipPlane - nearClipPlane);

    return Mat4f{
        cotHalfHorizontalFov, 0.0f,               0.0f,        0.0f,
        0.0f,                 cotHalfVerticalFov, 0.0f,        0.0f,
        0.0f,                 0.0f,               depthCoeff1, depthCoeff2,
        0.0f,                 0.0f,               -1.0f,       0.0f
    };
}

}

#endif //MATH_HPP
