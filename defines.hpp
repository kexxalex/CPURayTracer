#pragma once
#include <cmath>
#include <random>

using Float = float;

constexpr static Float PI = 3.14159265358979323846;
constexpr static Float TWO_PI = 2.0*PI;
constexpr static Float INV_PI = 1.0 / PI;


inline Float UniRand() {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    static thread_local std::uniform_real_distribution<Float> distr(0, 1);

    return distr(gen);
}