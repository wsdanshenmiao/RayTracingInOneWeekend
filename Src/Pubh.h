#pragma once
#ifndef __PUBH__H__
#define __PUBH_H__

#include <random>
#include <format>
#include <memory>
#include <array>
#include <vector>
#include "Math/Interval.h"
#include "Math/Vector.h"

namespace DSM{
    constexpr float PI = 3.14159265358979323846f;


    inline float DegreesToRadians(float degrees) noexcept
    {
        return degrees * PI / 180.0f;
    }


    inline float RadiansToDegrees(float radians) noexcept
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

    inline Vector3f RandomVector3f(float min = 0, float max = 1)
    {
        return Vector3f{RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max)};
    }

    inline Vector3f RandomUnitVector3f()
    {
        auto ret = RandomVector3f(-1.0f, 1.0f);
        auto sqrLen = ret.SqrMagnitude();
        if (1e-160 < sqrLen && sqrLen < 1) {
            ret /= std::sqrt(sqrLen);
        }
        return ret;
    }

    // 在法线方向上半球的随机向量
    inline Vector3f RandomOnHemiSphere(const Vector3f& normal)
    {
        auto ret = RandomUnitVector3f();
        ret = normal * ret < 0 ? -ret : ret;
        return ret;
    }
    
}

#endif
