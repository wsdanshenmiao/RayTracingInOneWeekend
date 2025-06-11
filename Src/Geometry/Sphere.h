#pragma once
#ifndef __SPHERE__H__
#define __SPHERE__H__

#include "Hittable.h"

namespace DSM {
    
    class Sphere : public Hittable
    {
    public:
        // 静态的球
        Sphere(Vector3f center, float radius, std::shared_ptr<Material> mat) noexcept
            :m_Center(center, {}), m_Radius(std::max(0.f, radius)), m_Material(mat) {
        }
		// 动态的球
        Sphere(Vector3f center0, Vector3f center1, float radius, std::shared_ptr<Material> mat) noexcept
            :m_Center(center0, center1 - center0), m_Radius(std::max(0.f, radius)), m_Material(mat) {
        }

        virtual bool Hit(const Ray& ray, HitRecord& hitRecord, Intervalf interval = Intervalf{}) const override;
    
    private:
        Ray m_Center;
        float m_Radius;
        std::shared_ptr<Material> m_Material;
    };
}

#endif

