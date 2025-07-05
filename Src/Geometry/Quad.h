#pragma once
#ifndef __QUAD_H__
#define __QUAD_H__

#include "Hittable.h"
#include "AABB.h"

namespace DSM {
    class HittableList;

    class Quad : public Hittable
    {
    public:
        Quad(Vector3f q, Vector3f u, Vector3f v, std::shared_ptr<Material> mat) noexcept;

        std::optional<HitRecord> Hit(const Ray& ray, Intervalf interval = Intervalf{}) const override;
        AABB BoundingBox() const noexcept override { return m_BoundingBox; }

        float PDFValue(const Vector3f& origin, const Vector3f& direction) const override;
        Vector3f Random(const Vector3f& origin) const override;

    private:
        Vector3f m_Q{};
        Vector3f m_U{}, m_V{};
        std::shared_ptr<Material> m_Mat;
        AABB m_BoundingBox{};

        Vector3f m_Normal{};
        float m_D;  // 用于计算射线与平面的交点
        Vector3f m_W;  // 用于计算交点是否在四边形内

        float m_Area;
    };

    
    namespace Geometry
    {
        std::shared_ptr<HittableList> Box(const Vector3f& a, const Vector3f& b, std::shared_ptr<Material> mat);
    } // namespace Geometry
    
    
} // namespace DSM 


#endif