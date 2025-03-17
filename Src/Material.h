#pragma once
#ifndef __MATERIAL__H__
#define __MATERIAL__H__

#include "Color.h"

namespace DSM {
    class Ray;
    struct HitRecord;
    
    struct Material
    {
        virtual ~Material() = default;

        virtual bool Scatter(const Ray& ray, const HitRecord& record, Color& attenuation, Ray& scattered) = 0;
    };


    class LambertMat : public Material
    {
    public:
        LambertMat(const Color& albedo) noexcept;
        virtual bool Scatter(const Ray& ray, const HitRecord& record, Color& attenuation, Ray& scattered) override;
        
    private:
        Color m_Albedo;
    };

    class MetalMat : public Material
    {
    public:
        MetalMat(const Color& albedo, float roughness = 0.4f) noexcept;
        virtual bool Scatter(const Ray& ray, const HitRecord& record, Color& attenuation, Ray& scattered) override;

    private:
        Color m_Albedo;
        float m_Roughness;  // 金属的粗糙程度
    };

    class DielectricMat : public Material
    {
    public:
        DielectricMat(float refractiveIndex) noexcept;
        virtual bool Scatter(const Ray& ray, const HitRecord& record, Color& attenuation,Ray& scattered) override;

    private:
        static float Reflectance(float cos, float refractionIndex) noexcept;
        
    private:
        float m_RefractiveIndex;
    };
    
}


#endif