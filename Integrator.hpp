#pragma once
#include "Vector.hpp"
#include "Object.hpp"
#include <vector>

constexpr int W = 1280;
constexpr int H = 800;
constexpr int N = 128;
constexpr int BOUNCES = 6;


Vec3 Li(const std::vector<const Object*> &objects, const Ray& ray, Float Prr = 1, uint32_t D=0, const void* const prev = nullptr) {
    if (D >= BOUNCES)
        return Vec3(PI);

    Interaction interaction(&ray);
    for (const Object* obj : objects) {
        if (obj != prev)
            obj->hit(&interaction);
    }

    if (!interaction.type)
        return Vec3(PI);

    Vec3 f = interaction.object->material->emission;

    Float rr = UniRand();
    if (rr < Prr) {
        interaction.update();
        const Object * object = interaction.object;
        const BxDF& material = *(object->material);
        const Vec3 &normal = interaction.normal;
        const Vec3 &position = interaction.position;
        const Float t = interaction.t;

        const Float inv_Prr = 1 / Prr;

        const Ray r(position, material.reflect(ray.direction, normal));
        const Float rho = material.rho(ray.direction, normal, r.direction);
        f += (material.f(ray.direction, r.direction) 
            * Li(objects, r, Prr*rho, D+1, object)
            * (rho * inv_Prr));
    }

    return f;
}