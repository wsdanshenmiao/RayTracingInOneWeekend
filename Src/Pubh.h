#pragma once
#ifndef __PUBH__H__
#define __PUBH_H__
#include <random>


constexpr float PI = 3.14159265358979323846f;


inline float DegreesToRadians(float degrees)
{
    return degrees * PI / 180.0f;
}


inline float RadiansToDegrees(float radians)
{
    return radians * 180.0f / PI;
}

inline float RandomFloat(float min = 0, float max = 1)
{
    std::random_device seed{};
    std::mt19937_64 gen{seed()};
    std::uniform_real_distribution<float> dist{min, max};
    return dist(gen);
}

#endif
