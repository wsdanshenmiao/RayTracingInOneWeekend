#pragma once
#ifndef __ONB_H__
#define __ONB_H__

#include "Vector.h"

namespace DSM {
    
    class ONB
    {
    public:
        ONB(const Vector3f& n)
        {
            m_W = n.Normalized();
            Vector3f a = (std::abs(m_W[0]) > 0.9f ? Vector3f{0, 1, 0} : Vector3f{1, 0, 0});
            m_V = Vector3f::Cross(m_W, a).Normalized();
            m_U = Vector3f::Cross(m_W, m_V);
        }

        Vector3f GetU() const noexcept { return m_U; }
        Vector3f GetV() const noexcept { return m_V; }
        Vector3f GetW() const noexcept { return m_W; }

        Vector3f Transform(const Vector3f& v) const noexcept
        {
            return v[0] * m_U + v[1] * m_V + v[2] * m_W;
        }

    private:
        Vector3f m_U, m_V, m_W;
    };
} // namespace DSM 


#endif