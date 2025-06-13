#pragma once
#ifndef __HITTABLE__H__
#define __HITTABLE__H__

#include "../Ray.h"
#include "../Math/Interval.h"
#include <optional>

namespace DSM {
    struct Material;
    class AABB;
    
    
    // 相交记录
    struct HitRecord
    {
        Vector3f m_Pos;
        Vector3f m_Normal;
        Vector2f m_UV;
        float m_Time;
        bool m_FrontFace;
        std::shared_ptr<Material> m_Material;

        void SetFaceNormal(const Ray& ray, const Vector3f& n)
        {
            m_FrontFace = (ray.GetDirection() * n) < 0;    // 光线是否在物体外部
            m_Normal = m_FrontFace ? n : -n;
        }
    };
    
    struct Hittable
    {
        virtual ~Hittable() = default;
        virtual std::optional<HitRecord> Hit(const Ray& ray, Intervalf interval = Intervalf{}) const = 0;
        virtual AABB BoundingBox() const noexcept = 0;
    };
}

#endif
