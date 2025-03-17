#pragma once
#ifndef __CAMERA__H__
#define __CAMERA__H__

#include "Math/Vector.h"

namespace DSM {
    struct HittableList;
    class Ray;
    class Color;
    
    class Camera
    {
    public:
        explicit Camera(float aspectRatio = 1, std::uint32_t width = 400, std::uint32_t samplePerPixel = 10) noexcept;

        void Render(const HittableList& world)const;


    private:
        Ray GetRay(std::uint32_t x, std::uint32_t y)const;
        Color GetRayColor(const Ray& ray, const HittableList& world, int depth)const;
        Vector2f GetSquare() const;


    public:
        float m_AspectRatio = 1;
        std::uint32_t m_SamplePerPixel = 10;
        std::uint32_t m_Width = 400;
        int m_MaxDepth = 10;        // 限制光线追踪最大深度
        
    private:
        std::uint32_t m_Height;
        Vector3f m_Pos;
        Vector3f m_StartPixelCenter;
        Vector3f m_PixelDeltaU;
        Vector3f m_PixelDeltaV;
    };


    
}


#endif