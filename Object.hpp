#pragma once
#include "Vector.hpp"
#include "Ray.hpp"
#include "Material.hpp"

enum TYPE { NONE, SPHERE, TRIANGLE, PLANE };

struct Interaction;

class Object {
public:
    Vec3 position;
    const BxDF * material{ nullptr };

    Object() = delete;
    constexpr Object(const Vec3 &P, const BxDF * const M) : position(P), material(M) {}

    virtual bool hit(Interaction * const interaction) const = 0;
    virtual constexpr Vec3 normalAt(const Interaction * const interaction) const = 0;
};

struct Interaction {
    TYPE type{NONE};
    const Ray* ray;
    Float t;

    Vec3 uv; // coord. of crossing relative to object coordinates
    const Object* object;

    Vec3 position;
    Vec3 normal;

    Interaction() = delete;

    constexpr Interaction(const Ray *r) : type(NONE), t(0), uv(0), object(nullptr), ray(r) {}

    inline void update() {
        position = ray->at(t);
        normal = object->normalAt(this);
    }
};

class Triangle : public Object {
public:
    Vec3 u, v;
    Vec3 true_normal;
    Vec3 corner_normals[3];

    Triangle() = delete;
    constexpr Triangle(const Vec3 &P, const Vec3 &U, const Vec3 &V, const BxDF * const M)
        : Object(P, M), u(U), v(V)
        , true_normal(U.cross(V))
        , corner_normals{true_normal, true_normal, true_normal}
    {}

    constexpr Triangle(const Vec3 &P, const Vec3 &U, const Vec3 &V, const Vec3 (&n)[3], const BxDF * const M)
        : Object(P, M), u(U), v(V)
        , true_normal(U.cross(V))
        , corner_normals{n[0], n[1], n[2]}
    {}

    bool hit(Interaction * const interaction) const;
    constexpr Vec3 normalAt(const Interaction * const i) const {
        return (corner_normals[0] * (1 - i->uv.x - i->uv.y) + corner_normals[1] * i->uv.x + corner_normals[2] * i->uv.y).normalize();
    }

};

class Sphere : public Object {
public:
    Float radius;

    Sphere() = delete;
    constexpr Sphere(const Vec3 &P, const Float R, const BxDF * const M) : Object(P, M), radius(R) {}

    bool hit(Interaction * const interaction) const;

    constexpr Vec3 normalAt(const Interaction * const i) const { return (i->position - position).normalize(); }
};

class Plane : public Object {
public:
    /*
        position is the normal!
    */
    Float hesse_const;

    Plane() = delete;
    constexpr Plane(const Vec3 &N, Float d, const BxDF * const M) : Object(N.normalize(), M), hesse_const(d / N.norm()) {}

    bool hit(Interaction * const interaction) const;
    constexpr Vec3 normalAt(const Interaction * const interaction) const { return position; }
};