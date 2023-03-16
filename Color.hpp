#include "Vector.hpp"

constexpr Float clamp(Float x, Float m=0, Float M=1) {
    return std::min(std::max(x, m), M);
}

constexpr Vec3 gammaCorrect(const Vec3 &x) {
    return Vec3(
        std::pow(x.r, 1/2.2),
        std::pow(x.g, 1/2.2),
        std::pow(x.b, 1/2.2));
}

constexpr Vec3 ACESFilm(const Vec3 &x) {
    Float a = 2.51;
    Float b = 0.03;
    Float c = 2.43;
    Float d = 0.59;
    Float e = 0.14;
    Vec3 color = (x*(x*a+Vec3(b)))/(x*(x*c+Vec3(d))+Vec3(e));
    return gammaCorrect(color);
}