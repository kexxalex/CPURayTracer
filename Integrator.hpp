#pragma once
#include "Vector.hpp"
#include "Object.hpp"
#include <vector>

constexpr int W = 1920;
constexpr int H = 1080;
constexpr int N = 32;
constexpr int BOUNCES = 16;

static constexpr Vec3 AMBIENT = Vec3(0.9, 0.95, 1.0) * PI * 0.5;


Vec3 Li(const std::vector<const Object*> &objects, const Ray& ray, Float Prr = 1, Float eta = 1, uint32_t D=0, const void* const prev = nullptr) {
    if (D >= BOUNCES)
        return AMBIENT;

    Interaction interaction(&ray);
    for (const Object* obj : objects) {
        if (obj != prev)
            obj->hit(&interaction);
    }

    if (!interaction.type)
        return AMBIENT;

    Vec3 f(0);

    Float rr = UniRand();
    if (rr < Prr) {
        interaction.update();
        const Object * object = interaction.object;
        const BxDF& material = *interaction.object->material;
        const Vec3 &normal = interaction.normal;
        const Vec3 &position = interaction.position;
        const Float t = interaction.t;

        const Float inv_Prr = 1 / Prr;

        Float rnd = UniRand();
        Float Fr = fresnel(ray.direction, normal, 1.0, material.eta);

        const Ray r(position, material.reflect(rnd, Fr, ray.direction, normal));
        const Float rho = material.rho(rnd, Fr, normal, r.direction);
        const Vec3 color = material.f() * (rho * inv_Prr);
        f += color * Li(objects, r, Prr*color.max(), 1.0, D+1, object);
    }

    return f;
}