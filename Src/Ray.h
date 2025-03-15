#pragma once
#ifndef __RAY__H__
#define __RAY__H__

#include "Math/Vector.h"

namespace DSM {
    class Ray
    {
    public:
        Ray() noexcept;
        Ray(Vector3f origin, Vector3f direction) noexcept;
        
        const Vector3f& GetDirection() const noexcept;
        const Vector3f& GetOrigin() const noexcept;

        Vector3f At(float t) const noexcept;

    private:
        Vector3f m_Origin;
        Vector3f m_Direction;
    };

    
}


#endif