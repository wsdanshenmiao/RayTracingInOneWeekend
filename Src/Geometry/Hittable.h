#pragma once
#ifndef __HITTABLE__H__
#define __HITTABLE__H__

#include "../Ray.h"
#include "../Math/Interval.h"

namespace DSM {
    // 相交记录
    struct HitRecord
    {
        Vector3f m_Pos;
        Vector3f m_Normal;
        float m_Time;
        bool m_FrontFace;

        void SetFaceNormal(const Ray& ray, const Vector3f& n)
        {
            m_FrontFace = (ray.GetDirection() * n) < 0;    // 光线是否在物体外部
            m_Normal = m_FrontFace ? n : -n;
        }
    };
    
    struct Hittable
    {
        virtual ~Hittable() = default;
        virtual bool Hit(const Ray& ray, HitRecord& hitRecord, Intervalf interval = Intervalf{}) const = 0;
    };
}

#endif
