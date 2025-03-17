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

    DielectricMat::DielectricMat(float refractiveIndex) noexcept
        :m_RefractiveIndex(refractiveIndex){
    }

    bool DielectricMat::Scatter(const Ray& ray, const HitRecord& record, Color& attenuation, Ray& scattered)
    {
        attenuation = Color{1,1,1};
        auto ri = record.m_FrontFace ? 1.f / m_RefractiveIndex : m_RefractiveIndex;   // 判断在内部还是外部，从而判断折射率
        auto unitDir = ray.GetDirection().Normalized();
        float cosTheta = std::min(-unitDir * record.m_Normal, 1.0f);
        float sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = ri * sinTheta > 1.0;
        Vector3f direction;

        if (cannotRefract || Reflectance(cosTheta, ri) > RandomFloat())
            direction = Vector3f::Reflect(unitDir, record.m_Normal);
        else
            direction = Vector3f::Refract(unitDir, record.m_Normal, ri);
        scattered = Ray{record.m_Pos, direction};
        return true;
    }

    float DielectricMat::Reflectance(float cos, float refractionIndex) noexcept
    {
        auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cos),5.f);
    }
}
