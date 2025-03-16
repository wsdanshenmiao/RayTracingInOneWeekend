#include "Camera.h"

namespace DSM{
    Camera::Camera(float aspectRatio, int width) noexcept
        :m_AspectRatio(aspectRatio), m_Width(width), m_Height(width / aspectRatio){
        
        float focalLength = 1;  // 焦点到视口中心的距离
        float viewportHeight = 2;
        float viewportWidth = viewportHeight * (float(m_Width) / m_Height);
        Vector3f cameraPos = Vector3f::Zero();
        // 视口向量
        Vector3f viewportU = {viewportWidth, 0, 0};
        Vector3f viewportV = {0, -viewportHeight, 0};    // v从上到下
        // 每个像素在视口中的比例
        m_PixelDeltaU = viewportU / float(m_Width);
        m_PixelDeltaV = viewportV / float(m_Height);
        // 左上角为像素起点
        Vector3f startPixel = cameraPos + Vector3f({0, 0, focalLength}) - viewportU / 2.0f - viewportV / 2.0f;
        m_StartPixelCenter = startPixel + (m_PixelDeltaU + m_PixelDeltaV) * 0.5f;
    }

    void Camera::Render(const HittableList& world) const
    {
        std::cout << "P3\n" << m_Width << ' ' << m_Height << "\n255\n";

        for (int j = 0; j < m_Height; j++) {
            std::clog << "\rScanlines remaining: " << (m_Height - j) << ' ' << std::flush;
            for (int i = 0; i < m_Width; i++) {
                auto pixelCenter = m_StartPixelCenter + float(i) * m_PixelDeltaU + float(j) * m_PixelDeltaV;
                auto ray = Ray(m_Pos, pixelCenter - m_Pos);
                
                auto color = GetColor(ray, world);
                std::cout << color.R() << ' ' << color.G() << ' ' << color.B() << "\n";
            }
        }
    }

    Color Camera::GetColor(const Ray& ray, const HittableList& world) const
    {
        HitRecord hitRecord;
        if (world.Hit(ray,hitRecord)) {
            auto normal = hitRecord.m_Normal;
            return 0.5 * (Color(normal[0], normal[1], normal[2]) + Color(1, 1, 1));
        }
        
        float y = ray.GetDirection().Normalized()[1];
        float a = 0.5f * (y + 1);  // [0, 1]
        return (1.0f - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }
}
