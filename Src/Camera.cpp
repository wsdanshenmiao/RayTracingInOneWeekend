#include "Camera.h"
#include "Pubh.h"
#include "Geometry/HittableList.h"
#include "Color.h"
#include "Material.h"


namespace DSM{
    Camera::Camera(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel) noexcept
        :m_AspectRatio(aspectRatio), m_Width(width), m_Height(width / aspectRatio), m_SamplePerPixel(samplePerPixel){
    }

    // 渲染场景
    void Camera::Render(const HittableList& world)
    {
        UpdateCamera();
        
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
                auto finalColor = color.GetColor();
                std::cout << std::format("{} {} {}\n", finalColor[0], finalColor[1], finalColor[2]);
            }
        }
    }

    void Camera::UpdateCamera()
    {
        m_Height = m_Width / m_AspectRatio;
        m_Height = (m_Height < 1) ? 1 : m_Height;
        m_Pos = m_Lookfrom;
        
        auto theta = DegreesToRadians(m_Vfov);
        auto h = std::tan(theta / 2);
        auto viewportHeight = 2 * h * m_FocusDist;
        float viewportWidth = viewportHeight * (float(m_Width) / m_Height);

        Vector3f front = (m_Lookat - m_Lookfrom).Normalized();  // 前方
        Vector3f right = Vector3f::Cross(front, m_Vup).Normalized();    // 右侧
        Vector3f down = Vector3f::Cross(front, right);
        
        // 视口向量
        Vector3f viewportU = viewportWidth * right;
        Vector3f viewportV = viewportHeight * down;    // down从上到下
        // 每个像素在视口中的比例
        m_PixelDeltaU = viewportU / float(m_Width);
        m_PixelDeltaV = viewportV / float(m_Height);
        // 左上角为像素起点
        Vector3f startPixel = m_Pos + m_FocusDist * front - viewportU * 0.5f - viewportV * 0.5f;
        m_StartPixelCenter = startPixel + (m_PixelDeltaU + m_PixelDeltaV) * 0.5f;

        auto defocusRadius = m_FocusDist * std::tan(DegreesToRadians(m_DefocusAngle / 2));
        m_DefocusDiskU = right * defocusRadius;
        m_DefocusDiskV = -down * defocusRadius;
    }

    // 获取某个像素处的光线
    Ray Camera::GetRay(std::uint32_t x, std::uint32_t y) const
    {
        auto offset = GetSquare();
        auto pixelSample = m_StartPixelCenter + (float(x) + offset[0]) * m_PixelDeltaU + (float(y) + offset[1]) * m_PixelDeltaV;
        auto rayOrigin = (m_DefocusAngle <= 0) ? m_Pos : DefocusDiskSample();
        return Ray(rayOrigin, pixelSample - rayOrigin);
    }

    // 获取光线采样到的颜色
    Color Camera::GetRayColor(const Ray& ray, const HittableList& world, int depth) const
    {
        if (depth < 0) {
            return Color{0, 0, 0};
        }
        auto interval = Intervalf{0.001f, std::numeric_limits<float>::max()};
        HitRecord hitRecord;
        if (world.Hit(ray,hitRecord, interval)) {
            Ray scattered{};
            Color attenuation{};
            if (hitRecord.m_Material->Scatter(ray, hitRecord, attenuation, scattered)) {
                return attenuation * GetRayColor(scattered, world, depth - 1); // 添加材质
            }
            return Color{0, 0, 0};
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

    Vector3f Camera::DefocusDiskSample() const
    {
        // Returns a random point in the camera defocus disk.
        auto p = RandomInUnitDisk();
        return m_Pos + (p[0] * m_DefocusDiskU) + (p[1] * m_DefocusDiskV);
    }
}
