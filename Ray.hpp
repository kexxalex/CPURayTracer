#pragma once
#include "Vector.hpp"


struct Ray {
    Vec3 position;
    Vec3 direction;

    Ray() : position(0), direction(0) {}

    static constexpr Ray NormalizedRay(const Vec3 &P, const Vec3 &D) {
        return Ray(P, D / D.norm());
    }

    constexpr Ray(const Vec3 &p, const Vec3 &d) : position(p), direction(d) {}
    // constexpr Ray(const Ray &ray) : position(ray.position), direction(ray.direction) {}

    constexpr Vec3 at(Float t) const { return position + direction * t; }
};