#pragma once
#include "defines.hpp"
#include <iostream>
#include <iomanip>

union Vec2 {
    struct {
        Float x, y;
    };
    struct {
        Float u, v;
    };

    constexpr Vec2() : x(0), y(0) {}

    explicit constexpr Vec2(Float scalar) : x(scalar), y(scalar) {}
    constexpr Vec2(Float X, Float Y) : x(X), y(Y) {}
    constexpr Vec2(const Vec2 &v) : x(v.x), y(v.y) {}

    constexpr Vec2 operator+(const Vec2 &v) const { return Vec2(x + v.x, y + v.y); }
    constexpr Vec2 operator-(const Vec2 &v) const { return Vec2(x - v.x, y - v.y); }
    constexpr Vec2 operator-() const { return Vec2(-x, -y); }

    constexpr Vec2 operator*(const Float scalar) const { return Vec2(x * scalar, y * scalar); }
    constexpr Vec2 operator/(const Float scalar) const {
        const Float inv = Float(1) / scalar;
        return Vec2(x * inv, y * inv);
    }


    constexpr void operator+=(const Vec2 &v) { x += v.x; y += v.y; }
    constexpr void operator-=(const Vec2 &v) { x -= v.x; y -= v.y; }

    constexpr void operator*=(const Float scalar) { x *= scalar; y *= scalar; }
    constexpr void operator/=(const Float scalar) {
        const Float inv = Float(1) / scalar;
        x *= inv; y *= inv;
    }

    constexpr Float dot(const Vec2 &v) const { return x*v.x + y*v.y; }
    constexpr Float norm_sqr() const { return dot(*this); }
    constexpr Float norm() const { return sqrt(norm_sqr()); }

    constexpr Vec2 normalize() const { return (*this) / norm(); }
};

union Vec3 {
    struct {
        Float x, y, z;
    };
    struct {
        Float r, g, b;
    };

    constexpr Vec3() : x(0), y(0), z(0) {}

    explicit constexpr Vec3(Float scalar) : x(scalar), y(scalar), z(scalar) {}
    constexpr Vec3(Float X, Float Y, Float Z) : x(X), y(Y), z(Z) {}
    constexpr Vec3(const Vec3 &v) : x(v.x), y(v.y), z(v.z) {}

    constexpr Vec3 operator+(const Vec3 &v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    constexpr Vec3 operator-(const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    constexpr Vec3 operator-() const { return Vec3(-x, -y, -z); }

    constexpr Vec3 operator*(const Float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    constexpr Vec3 operator*(const Vec3 &v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    constexpr Vec3 operator/(const Float scalar) const {
        const Float inv = Float(1) / scalar;
        return Vec3(x * inv, y * inv, z * inv);
    }
    constexpr Vec3 operator/(const Vec3 &v) const {
        return Vec3(x / v.x, y / v.y, z / v.z);
    }


    constexpr void operator+=(const Vec3 &v) { x += v.x; y += v.y; z += v.z; }
    constexpr void operator-=(const Vec3 &v) { x -= v.x; y -= v.y; z -= v.z; }

    constexpr void operator*=(const Float scalar) { x *= scalar; y *= scalar; z *= scalar; }
    constexpr void operator/=(const Float scalar) {
        const Float inv = Float(1) / scalar;
        x *= inv; y *= inv; z *= inv;
    }

    constexpr Float dot(const Vec3 &v) const { return x*v.x + y*v.y + z*v.z; }
    constexpr Vec3 cross(const Vec3 &v) const {
        return Vec3(
            y*v.z - z*v.y,
            z*v.x - x*v.z,
            x*v.y - y*v.x);
    }
    constexpr Float norm_sqr() const { return dot(*this); }
    constexpr Float norm() const { return sqrt(norm_sqr()); }

    constexpr Vec3 normalize() const { return (*this) / norm(); }
    
};

constexpr Vec3 reflect_n(const Vec3 &I, const Vec3 &N) {
    return I - N * (Float(2)*N.dot(I));
}

constexpr Vec3 reflect(const Vec3 &I, const Vec3 &N) {
    return I - N * (Float(2)*N.dot(I) / N.norm_sqr());
}

inline Vec3 random_unit_vector() {
    const Float phi = UniRand() * PI;
    const Float theta = UniRand() * TWO_PI;

    return Vec3(sin(phi)*sin(theta), sin(phi)*cos(theta), cos(phi));
}

inline Vec3 random_hemi_vector(const Vec3 &n) {
    const Vec3 u = random_unit_vector();
    if (n.dot(u) < 0)
        return -u;
    else
        return u;
}


inline std::ostream &operator<<(std::ostream &os, const Vec3 &v) {
    os << std::fixed << std::showpos << std::setprecision(2);
    os << "( " << std::setw(4) << v.x << ", " << std::setw(4) << v.y << ", " << std::setw(4) << v.z << " )";
    return os;
}