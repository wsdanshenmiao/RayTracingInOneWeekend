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
        static std::random_device seed{};
        static std::mt19937_64 gen{seed()};
        std::uniform_real_distribution<float> dist{min, max};
        return dist(gen);
    }

    inline int RandomInt(int min = 0, int max = 1)
    {
        return int(RandomFloat((float)min, (float)max));
    }

    inline Vector3f RandomVector3f(float min = 0, float max = 1)
    {
        return Vector3f{RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max)};
    }

    inline Vector3f RandomUnitVector3f()
    {
        while(true){
            auto ret = RandomVector3f(-1.0f, 1.0f);
            auto sqrLen = ret.SqrMagnitude();
            if (1e-160 < sqrLen && sqrLen <= 1) {
                ret /= std::sqrt(sqrLen);
                return ret;
            }
        }
    }

    // 在法线方向上半球的随机向量
    inline Vector3f RandomOnHemiSphere(const Vector3f& normal)
    {
        auto ret = RandomUnitVector3f();
        ret = normal * ret < 0 ? -ret : ret;
        return ret;
    }

    inline Vector3f RandomInUnitDisk() {
        while (true) {
            auto p = Vector3f{RandomFloat(-1,1), RandomFloat(-1,1), 0};
            if (p.SqrMagnitude() < 1)
                return p;
        }
    }
    
    inline Vector3f RandomCosineDirection()
    {
        float r1 = RandomFloat();
        float r2 = RandomFloat();
        float phi = 2 * PI * r1;
        float x = std::cos(phi) * std::sqrt(r2);
        float y = std::sin(phi) * std::sqrt(r2);
        float z = std::sqrt(1 - r2);
        return Vector3f{x, y, z};
    }
}

#endif
