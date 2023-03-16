#pragma once
#include "Vector.hpp"

class BxDF {
protected:
    Vec3 R;
    Vec3 T;

public:
    constexpr BxDF(const Vec3& reflectance, const Vec3& transmission, Float eta)
        : R(reflectance)
        , T(transmission)
        , eta(eta) {}

    virtual inline Vec3 f() const = 0;
    virtual inline Float rho(Float rnd, Float Fr, const Vec3& N, const Vec3& O) const = 0;
    virtual inline Vec3 reflect(Float rnd, Float Fr, const Vec3& I, const Vec3& N) const = 0;

    Float eta;
};

class Lambertion : public BxDF {
public:
    constexpr Lambertion(const Vec3& R, const Vec3& T, Float eta = 1) : BxDF(R, T, eta) {}
    inline Vec3 f() const { return R; }
    inline Float rho(Float rnd, Float Fr, const Vec3& N, const Vec3& O) const {
        return INV_PI * N.dot(O);
    }
    inline Vec3 reflect(Float rnd, Float Fr, const Vec3& I, const Vec3& N) const {
        return random_hemi_vector(N);
    }
};

class Specular : public BxDF {
public:
    constexpr Specular(const Vec3& R, const Vec3& T, Float eta = 1.0, Float Rho = 1.0) : BxDF(R, T, eta), m_rho(Rho) {}
    inline Vec3 f() const { return R; }
    inline Float rho(Float rnd, Float Fr, const Vec3& N, const Vec3& O) const {
        return m_rho;
    }
    inline Vec3 reflect(Float rnd, Float Fr, const Vec3& I, const Vec3& N) const {
        return reflect_n(I, N);
    }
private:
    const Float m_rho;
};

class DiElectric : public BxDF {
public:
    constexpr DiElectric(const Vec3& R, const Vec3& T, Float eta = 1.0, Float r = 0.5)
        : BxDF(R, T, eta), m_roughness(r) {}

    inline Vec3 f() const { return R; }
    inline Float rho(Float rnd, Float Fr, const Vec3& N, const Vec3& O) const {
        if (rnd >= m_roughness)
            return (1.0 - Fr) + Fr * N.dot(O);
        else 
            return INV_PI * N.dot(O) * Fr;
    }
    inline Vec3 reflect(Float rnd, Float Fr, const Vec3& I, const Vec3& N) const {
        if (rnd >= m_roughness)
            return reflect_n(I, N);
        else
            return random_hemi_vector(N);
    }

private:
    Float m_roughness{ 0.0 };
};

inline Float fresnel(const Vec3 &I, const Vec3 &N, Float etaI, Float etaT) {
    Float cosThetaI = -I.dot(N);
    if (cosThetaI < 0) {
        std::swap(etaI, etaT);
        cosThetaI = -cosThetaI;
    }
    Float sinThetaI = sqrt(1-cosThetaI*cosThetaI);
    Float sinThetaT = etaI / etaT * sinThetaI;
    if (sinThetaT > 1)
        sinThetaT = 1;

    Float cosThetaT = sqrt(1-sinThetaT*sinThetaT);

    Float TI = etaT * cosThetaI;
    Float TT = etaT * cosThetaT;
    Float IT = etaI * cosThetaT;
    Float II = etaI * cosThetaI;

    Float Rparl = (TI - IT) / (TI + IT);
    Float Rperp = (II - TT) / (II + TT);

    return 0.5 * (Rparl * Rparl + Rperp * Rperp);
}

static constexpr Lambertion    DIFFUSE_WHITE( Vec3(1), Vec3(0));
static constexpr DiElectric DIELECTRIC_WHITE( Vec3(1), Vec3(0), 1.1, 0.1);
static constexpr DiElectric  DIELECTRIC_GOLD( Vec3(244, 202, 104) / 255.0, Vec3(0), 1.1, 0.0);
static constexpr DiElectric           GROUND( Vec3(1), Vec3(0), 1.0, 0.5);