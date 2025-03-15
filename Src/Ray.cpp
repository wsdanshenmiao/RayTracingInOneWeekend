#include "Ray.h"

namespace DSM {
    Ray::Ray() noexcept
        :m_Direction(0), m_Origin(0){
    }

    Ray::Ray(Vector3f origin, Vector3f direction) noexcept
        :m_Direction(direction.Normalized()), m_Origin(origin){
    }

    const Vector3f& Ray::GetDirection() const noexcept
    {
        return m_Direction;
    }

    const Vector3f& Ray::GetOrigin() const noexcept
    {
        return m_Origin;
    }

    Vector3f Ray::At(float t) const noexcept
    {
        return m_Origin + t * m_Direction;
    }
}
