#pragma once
#ifndef __SPHERE__H__
#define __SPHERE__H__

#include "Hittable.h"

namespace DSM {
    class Sphere : public Hittable
    {
    public:
        Sphere(Vector3f center, float radius) noexcept;
        
        virtual bool Hit(const Ray& ray, HitRecord& hitRecord, Intervalf interval = Intervalf{}) const override;
    
    private:
        Vector3f m_Center;
        float m_Radius;
    };
}

#endif

