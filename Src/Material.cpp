#include "Material.h"
#include "Pubh.h"
#include "Geometry/Hittable.h"
#include "Math/PDF.h"

namespace DSM {
    bool LambertMat::Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const
    {
        srec.m_Attenuation = m_Albedo->Sample(record.m_UV);
        srec.m_PDF = std::make_shared<CosinePDF>(record.m_Normal); 
        srec.m_SkipPDF = false;
        return true;
    }

    float LambertMat::ScatterPDF(const Ray &ray, const HitRecord &record, const Ray &scatter) const
    {
        float costheta = Vector3f::Dot(record.m_Normal, scatter.GetDirection().Normalized());
        return std::max(0.f, costheta / PI);
    }

    MetalMat::MetalMat(const Color& albedo, float roughness ) noexcept
        :m_Albedo(albedo), m_Roughness(std::min(1.f, roughness)){
    }

    bool MetalMat::Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const
    {
        auto reflectDir = Vector3f::Reflect(ray.GetDirection(), record.m_Normal);
        reflectDir = reflectDir.Normalized() + RandomUnitVector3f() * m_Roughness;  // 将光线随机散射

        srec.m_Attenuation = m_Albedo;
        srec.m_PDF = nullptr;
        srec.m_SkipPDF = true;
        srec.m_SkipPDFRay = Ray{record.m_Pos, reflectDir, ray.GetTime()};

        return true;
    }

    DielectricMat::DielectricMat(float refractiveIndex) noexcept
        :m_RefractiveIndex(refractiveIndex){
    }

    bool DielectricMat::Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const
    {
        srec.m_Attenuation = Color{1,1,1};
        srec.m_PDF = nullptr;
        srec.m_SkipPDF = true;

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
        srec.m_SkipPDFRay = Ray{record.m_Pos, direction.Normalized(), ray.GetTime()};

        return true;
    }

    float DielectricMat::Reflectance(float cos, float refractionIndex) noexcept
    {
        auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cos),5.f);
    }


    bool DiffuseLightMat::Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const
    {
        return false;
    }

    Color DiffuseLightMat::Emitted(const HitRecord &record, const Vector2f &uv, const Vector3f &pos)
    {
        return record.m_FrontFace ? m_Texture->Sample(uv) : Color{0,0,0};
    }

    bool IsotropicMat::Scatter(const Ray& ray, const HitRecord& rec, ScatterRecord& srec) const
    {
        srec.m_Attenuation = m_Texture->Sample(rec.m_UV);
        srec.m_PDF = std::make_shared<CosinePDF>(rec.m_Normal); 
        srec.m_SkipPDF = false;
        return true;
    }

    float IsotropicMat::ScatterPDF(const Ray &ray, const HitRecord &record, const Ray &scatter) const
    {
        return 1.f / (4 * PI);
    }
}
