#pragma once
#ifndef __CONSTANTMEDIUM_H__
#define __CONSTANTMEDIUM_H__

#include "Hittable.h"
#include "../Material.h"


namespace DSM {
    class Texture;

    class ConstantMedium : public Hittable
    {
    public:
        ConstantMedium(std::shared_ptr<Hittable> boundary, float density, std::shared_ptr<Texture> tex);
        ConstantMedium(std::shared_ptr<Hittable> boundary, float density, const Color& col);

        std::optional<HitRecord> Hit(const Ray& ray, Intervalf interval = Intervalf{}) const override;
        AABB BoundingBox() const noexcept { return m_Boundary->BoundingBox(); }

    private:
        std::shared_ptr<Hittable> m_Boundary;
        std::shared_ptr<Material> m_PhaseFunc;
        float m_NegInvDensity{};
    };
} // namespace DSM 


#endif