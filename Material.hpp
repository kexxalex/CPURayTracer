#include "Vector.hpp"

class BxDF {
public:
    Vec3 emission;
    constexpr BxDF(const Vec3& reflectance, const Vec3& transmission)
        : R(reflectance)
        , T(transmission) {}

    virtual constexpr Vec3 f(const Vec3& I, const Vec3& O) const = 0;
    virtual constexpr Float rho(const Vec3& I, const Vec3& N, const Vec3& O) const = 0;
    virtual inline Vec3 reflect(const Vec3& I, const Vec3& N) const = 0;

protected:
    Vec3 R;
    Vec3 T;
};

class Lambertion : public BxDF {
public:
    constexpr Lambertion(const Vec3& R, const Vec3& T) : BxDF(R, T) {}
    constexpr Vec3 f(const Vec3& I, const Vec3& O) const { return R; }
    constexpr Float rho(const Vec3& I, const Vec3& N, const Vec3& O) const {
        return INV_PI * O.dot(N);
    }
    inline Vec3 reflect(const Vec3& I, const Vec3& N) const {
        return random_hemi_vector(N);
    }
};

class Specular : public BxDF {
public:
    constexpr Specular(const Vec3& R, const Vec3& T, Float Rho = 1.0) : BxDF(R, T), m_rho(Rho) {}
    constexpr Vec3 f(const Vec3& I, const Vec3& O) const { return R; }
    constexpr Float rho(const Vec3& I, const Vec3& N, const Vec3& O) const {
        return m_rho; //O.dot(N);
    }
    inline Vec3 reflect(const Vec3& I, const Vec3& N) const {
        return reflect_n(I, N);
    }
private:
    const Float m_rho;
};

static constexpr Lambertion DIFFUSE_WHITE(Vec3(1), Vec3(0));
static constexpr Specular  SPECULAR_WHITE(Vec3(1, 0, 0), Vec3(0), 0.1);