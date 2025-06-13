#pragma once
#ifndef __CAMERA__H__
#define __CAMERA__H__

#include "Math/Vector.h"
#include "Image.h"

namespace DSM {
    class HittableList;
    class Ray;
    class Color;
    
    class Camera
    {
    public:
        explicit Camera(float aspectRatio = 1, std::uint32_t width = 400, std::uint32_t samplePerPixel = 10) noexcept;
        Camera(const Camera&) = default;
        Camera(Camera&&) noexcept = default;
        Camera& operator=(const Camera&) = default;
        Camera& operator=(Camera&&) noexcept = default;

        const Image& Render(const HittableList& world);
        const Image& GetImage() const { return m_Image; }

    private:
        void UpdateCamera();
        Ray GetRay(std::uint32_t x, std::uint32_t y)const;
        Color GetRayColor(const Ray& ray, const HittableList& world, int depth)const;
        Vector2f GetSquare() const;
        Vector3f DefocusDiskSample() const;
		std::vector<Color> Render(
			const HittableList& world,
			std::uint32_t beginW, std::uint32_t endW,
			std::uint32_t beginH, std::uint32_t endH);


    public:
        float m_AspectRatio = 1;
        std::uint32_t m_SamplePerPixel = 10;
        std::uint32_t m_Width = 400;
        int m_MaxDepth = 10;        // 限制光线追踪最大深度
        float m_Vfov = 90;
        Vector3f m_Lookfrom = Vector3f{0,0,0};  // 在哪看
        Vector3f m_Lookat = Vector3f{0,0,-1};   // 看哪里
        Vector3f m_Vup = Vector3f{0,1,0};
		float m_DefocusAngle = 0; // 焦散角度，也就是景深
		float m_FocusDist = 10; // 焦距
    
    private:
        std::uint32_t m_Height;
        Vector3f m_Pos;
        Vector3f m_StartPixelCenter;
        Vector3f m_PixelDeltaU;
        Vector3f m_PixelDeltaV;
        Vector3f m_DefocusDiskU;
        Vector3f m_DefocusDiskV;

        Image m_Image;
    };


    
}


#endif