#include "Material.h"
#include "Pubh.h"
#include "Geometry/Hittable.h"

namespace DSM {
    LambertMat::LambertMat(const Color& albedo) noexcept
        :m_Albedo(albedo){
    }

    bool LambertMat::Scatter(const Ray& ray, const HitRecord& record, Color& attenuation, Ray& scattered)
    {
        auto reflectDir = record.m_Normal + RandomUnitVector3f();   // 可能会出现零向量
        if (reflectDir.NearZero()) {    // 避免出现零向量
            reflectDir = record.m_Normal;
        }
        scattered = Ray{record.m_Pos, reflectDir};
        attenuation = m_Albedo;
        return true;
    }

    MetalMat::MetalMat(const Color& albedo, float roughness ) noexcept
        :m_Albedo(albedo), m_Roughness(std::min(1.f, roughness)){
    }

    bool MetalMat::Scatter(const Ray& ray, const HitRecord& record, Color& attenuation, Ray& scattered)
    {
        auto reflectDir = Vector3f::Reflect(ray.GetDirection(), record.m_Normal);
        reflectDir = reflectDir.Normalized() + RandomUnitVector3f() * m_Roughness;  // 将光线随机散射
        scattered = Ray{record.m_Pos, reflectDir};
        attenuation = m_Albedo;
        return reflectDir * record.m_Normal > 0;
    }
}
