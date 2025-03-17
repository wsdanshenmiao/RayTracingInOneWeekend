#include "Camera.h"
#include "Pubh.h"

namespace DSM{
    Camera::Camera(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel) noexcept
        :m_AspectRatio(aspectRatio), m_Width(width), m_Height(width / aspectRatio), m_SamplePerPixel(samplePerPixel){
        
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

    // 渲染场景
    void Camera::Render(const HittableList& world) const
    {
        std::cout << std::format("P3\n {} {} \n255\n", m_Width, m_Height);

        float invSamplePerPixel = 1.0f / m_SamplePerPixel;
        
        for (std::uint32_t j = 0; j < m_Height; j++) {
            std::clog << std::format("\rScanlines remaining: {} ", m_Height - j) << std::flush;
            for (std::uint32_t i = 0; i < m_Width; i++) {
                Color color{};
                for (std::uint32_t k = 0; k < m_SamplePerPixel; k++) {
                    auto ray = GetRay(i, j);
                    color += GetRayColor(ray, world, m_MaxDepth);
                }
                color *= invSamplePerPixel;
                std::cout << std::format("{} {} {}\n", color.R() ,color.G() ,color.B());
            }
        }
    }

    // 获取某个像素处的光线
    Ray Camera::GetRay(std::uint32_t x, std::uint32_t y) const
    {
        auto offset = GetSquare();
        auto pixelSample = m_StartPixelCenter + (float(x) + offset[0]) * m_PixelDeltaU + (float(y) + offset[1]) * m_PixelDeltaV;

        return Ray(m_Pos, pixelSample - m_Pos);
    }

    // 获取光线采样到的颜色
    Color Camera::GetRayColor(const Ray& ray, const HittableList& world, int depth) const
    {
        if (depth < 0) {
            return Color{0, 0, 0};
        }
        HitRecord hitRecord;
        if (world.Hit(ray,hitRecord, Intervalf{0.001f, std::numeric_limits<float>::max()})) {
            auto dir = RandomOnHemiSphere(hitRecord.m_Normal);
            return 0.5 * GetRayColor(Ray{hitRecord.m_Pos, dir}, world, depth - 1); // 假设吸收率为0.5
        }
        
        float y = ray.GetDirection().Normalized()[1];
        float a = 0.5f * (y + 1);  // [0, 1]
        return (1.0f - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }

    // 获取随机偏移
    Vector2f Camera::GetSquare() const
    {
        return Vector2f{RandomFloat() - 0.5f, RandomFloat() - 0.5f};
    }
}
