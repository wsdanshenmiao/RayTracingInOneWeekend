#pragma once
#ifndef __SPHERE__H__
#define __SPHERE__H__

#include "Hittable.h"
#include "AABB.h"

namespace DSM {
    
    class Sphere : public Hittable
    {
    public:
        // 静态的球
        Sphere(Vector3f center, float radius, std::shared_ptr<Material> mat) noexcept;
		// 动态的球
        Sphere(Vector3f center0, Vector3f center1, float radius, std::shared_ptr<Material> mat) noexcept;

        std::optional<HitRecord> Hit(const Ray& ray, Intervalf interval = Intervalf{}) const override;
        AABB BoundingBox() const noexcept override { return m_BoundingBox; }

    private:
        static Vector2f GetUV(Vector3f pos) noexcept;

    private:
        Ray m_Center;
        float m_Radius;
        AABB m_BoundingBox;
        std::shared_ptr<Material> m_Material;
    };
}

#endif

