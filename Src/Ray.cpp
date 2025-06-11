#include "Ray.h"

namespace DSM {
    Ray::Ray() noexcept
        :m_Direction(0), m_Origin(0), m_Time(0){
    }

    Ray::Ray(const Vector3f& origin, const Vector3f& direction, float time) noexcept
        :m_Direction(direction), m_Origin(origin), m_Time(time){
    }

    const Vector3f& Ray::GetDirection() const noexcept
    {
        return m_Direction;
    }

    const Vector3f& Ray::GetOrigin() const noexcept
    {
        return m_Origin;
    }

    const float& Ray::GetTime() const noexcept
    {
        return m_Time;
    }

    Vector3f Ray::At(float t) const noexcept
    {
        return m_Origin + t * m_Direction;
    }
}
