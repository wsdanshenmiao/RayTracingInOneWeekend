#pragma once
#ifndef __MATERIAL__H__
#define __MATERIAL__H__

#include "Color.h"
#include "Texture.h"

namespace DSM {
    class Ray;
    struct HitRecord;
    
    struct Material
    {
        virtual ~Material() = default;

        virtual bool Scatter(const Ray& ray, const HitRecord& record, Color& attenuation, Ray& scattered) const = 0;
    };


    class LambertMat : public Material
    {
    public:
        LambertMat(const Color& albedo) noexcept : m_Albedo(std::make_shared<SolidColorTexture>(TextureDesc{1, 1, 0}, albedo)) {}
        LambertMat(std::shared_ptr<Texture> albedo) noexcept :m_Albedo(albedo){}
        virtual bool Scatter(const Ray& ray, const HitRecord& record, Color& attenuation, Ray& scattered) const override;
        
    private:
        std::shared_ptr<Texture> m_Albedo;
    };

    class MetalMat : public Material
    {
    public:
        MetalMat(const Color& albedo, float roughness = 0.4f) noexcept;
        virtual bool Scatter(const Ray& ray, const HitRecord& record, Color& attenuation, Ray& scattered) const override;

    private:
        Color m_Albedo;
        float m_Roughness;  // 金属的粗糙程度
    };

    class DielectricMat : public Material
    {
    public:
        DielectricMat(float refractiveIndex) noexcept;
        virtual bool Scatter(const Ray& ray, const HitRecord& record, Color& attenuation,Ray& scattered) const override;

    private:
        static float Reflectance(float cos, float refractionIndex) noexcept;
        
    private:
        float m_RefractiveIndex;
    };
    
}


#endif