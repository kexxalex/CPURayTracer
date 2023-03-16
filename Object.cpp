#include "Object.hpp"

bool Triangle::hit(Interaction * const interaction) const {
    const Ray &ray = *(interaction->ray);
    const Float det = ray.direction.dot(true_normal);
    if (det >= 0) {
        return false;
    }

    const Vec3 delta = ray.position - position;
    const Float t = true_normal.dot(delta);
    if (interaction->type != TYPE::NONE && t <= interaction->t*det) {
        return false;
    }

    const Vec3 cross = delta.cross(ray.direction);
    const Float a = -cross.dot(v);
    const Float b = cross.dot(u);

    if (a > 0 || b > 0 || det > a || det > b) {
        return false;
    }

    const Float inv_det = 1/det;
    interaction->type = TYPE::TRIANGLE;
    interaction->t = -t*inv_det;
    interaction->uv = Vec3(a*inv_det, b*inv_det, 0);
    interaction->object = this;

    return true;
}

bool Sphere::hit(Interaction * const interaction) const {
    const Ray &ray = *(interaction->ray);
    // Assuming ray.direction is normalized
    const Vec3 delta = position - ray.position;
    const Float rDd = ray.direction.dot(delta);
    if (rDd < 0)
        return false;

    const Float disc = rDd*rDd + radius*radius - delta.norm_sqr();
    if (disc < 0)
        return false;

    Float t = rDd;

    if (disc > 0) {
        const Float disc_sqrt = sqrt(disc);
        t -= disc_sqrt;
    }

    if (interaction->type != TYPE::NONE && interaction->t < t)
        return false;

    interaction->type = TYPE::SPHERE;
    interaction->t = t;
    interaction->uv = Vec3(0, 0, 0);
    interaction->object = this;

    return true;
}

bool Plane::hit(Interaction * const interaction) const {
    const Ray &ray = *(interaction->ray);
    Float den = position.dot(ray.direction);
    if (den >= 0)
        return false;

    Float num = hesse_const - position.dot(ray.position);
    if (interaction->type != TYPE::NONE && num <= interaction->t*den)
        return false;
    
    interaction->type = TYPE::PLANE;
    interaction->t = num / den;
    interaction->uv = Vec3(0);
    interaction->object = this;

    return true;
}
