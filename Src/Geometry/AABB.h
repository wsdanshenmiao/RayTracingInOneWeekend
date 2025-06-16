#pragma once
#ifndef __AABB_H__
#define __AABB_H__

#include "../Math/Interval.h"
#include "../Ray.h"

namespace DSM {
    class Ray;

    class AABB 
    {
    public:
        AABB() noexcept = default;
        AABB(const Intervalf& x, const Intervalf& y, const Intervalf& z) noexcept
            : m_X(x), m_Y(y), m_Z(z) { PadToMinimums(); }
        AABB(const Vector3f& min, const Vector3f& max);
        AABB(const AABB&) noexcept = default;
        AABB(AABB&&) noexcept = default;
        AABB& operator=(const AABB&) noexcept = default;
        AABB& operator=(AABB&&) noexcept = default;

        bool operator==(const AABB& other) const noexcept = default;
        Intervalf operator[](std::size_t index) const;

        bool Hit(const Ray& ray, Intervalf rayT) const;
        std::size_t LongestAxis() const noexcept;

        static AABB Intersection(const AABB& box0, const AABB& box1);
        static AABB Uion(const AABB& box0, const AABB& box1);

    private:
        void PadToMinimums();

    private:
        Intervalf m_X{}, m_Y{}, m_Z{};
    };


} // namespace DSM

#endif