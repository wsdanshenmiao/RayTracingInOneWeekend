#pragma once
#ifndef __MATERIAL__H__
#define __MATERIAL__H__

#include "Color.h"
#include "Texture.h"
#include "Ray.h"

namespace DSM {
    struct HitRecord;
    struct PDF;

    struct ScatterRecord
    {
        Color m_Attenuation;
        std::shared_ptr<PDF> m_PDF;
        bool m_SkipPDF;
        Ray m_SkipPDFRay;
    };
    
    struct Material
    {
        virtual ~Material() = default;

        virtual bool Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const = 0;
        virtual Color Emitted(const HitRecord& record, const Vector2f& uv, const Vector3f& pos) { return Color{0,0,0}; }
        virtual float ScatterPDF(const Ray& ray, const HitRecord& record, const Ray& scatter) const { return 0; }
    };


    class LambertMat : public Material
    {
    public:
        LambertMat(const Color& albedo) noexcept : m_Albedo(std::make_shared<SolidColorTexture>(albedo)) {}
        LambertMat(std::shared_ptr<Texture> albedo) noexcept :m_Albedo(albedo){}
        bool Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const override;
        float ScatterPDF(const Ray& ray, const HitRecord& record, const Ray& scatter) const override;

    private:
        std::shared_ptr<Texture> m_Albedo;
    };

    class MetalMat : public Material
    {
    public:
        MetalMat(const Color& albedo, float roughness = 0.4f) noexcept;
        bool Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const override;

    private:
        Color m_Albedo;
        float m_Roughness;  // 金属的粗糙程度
    };

    class DielectricMat : public Material
    {
    public:
        DielectricMat(float refractiveIndex) noexcept;
        bool Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const override;

    private:
        static float Reflectance(float cos, float refractionIndex) noexcept;
        
    private:
        float m_RefractiveIndex;
    };
    
    // 光源
    class DiffuseLightMat : public Material
    {
    public:
        DiffuseLightMat(std::shared_ptr<Texture> tex) : m_Texture(tex) {}
        DiffuseLightMat(const Color& emit) : m_Texture(std::make_shared<SolidColorTexture>(emit)) {}

        bool Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const override;
        virtual Color Emitted(const HitRecord& record, const Vector2f& uv, const Vector3f& pos) override;

    private:
        std::shared_ptr<Texture> m_Texture{};
    };

    // 各向同性的材质
    class IsotropicMat : public Material
    {
    public:
        IsotropicMat(std::shared_ptr<Texture> tex) : m_Texture(tex) {}
        IsotropicMat(const Color& col) : m_Texture(std::make_shared<SolidColorTexture>(col)) {}


        bool Scatter(const Ray& ray, const HitRecord& record, ScatterRecord& srec) const override;
        
        float ScatterPDF(const Ray& ray, const HitRecord& record, const Ray& scatter) const override;
    
    private:
        std::shared_ptr<Texture> m_Texture{};
    };


}


#endif