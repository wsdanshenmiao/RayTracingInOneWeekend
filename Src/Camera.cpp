#include "Camera.h"
#include "Pubh.h"
#include "Geometry/HittableList.h"
#include "Color.h"
#include "Material.h"
#include <future>
#include <format>
#include <print>
#include "Math/PDF.h"


namespace DSM{
    Camera::Camera(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel) noexcept
        :m_AspectRatio(aspectRatio), 
        m_Width(std::max(width, 1u)),
        m_Height(std::max((std::uint32_t)(m_Width / aspectRatio), 1u)),
        m_SamplePerPixel(samplePerPixel),
        m_Image(width, width / aspectRatio){
    }

    // 渲染场景
    void Camera::Render(const Hittable& world, const Hittable& light)
    {
        UpdateCamera();

        // 计算需要分配的线程数
		const bool rowSplit = m_Width < m_Height;
        const std::size_t maxThreads = rowSplit ? m_Width : m_Height;

        const std::size_t hardwareThreads = std::thread::hardware_concurrency();
        const std::size_t threadCount = std::min(hardwareThreads > 0 ? 
            hardwareThreads : 2, maxThreads);

        const std::size_t blockSize = (m_Height + threadCount - 1) / threadCount;

        std::vector<std::future<std::vector<Color>>> results(threadCount);

        m_ProgressCount = threadCount;
        std::clog << "\rScanlines remaining: " << m_ProgressCount << ' ' << std::flush;

        for (std::size_t i = 0; i < threadCount; ++i) {
            std::uint32_t beginW = 0, endW = m_Width;
			std::uint32_t beginH = i * blockSize, endH = (i + 1) * blockSize;
            if (beginH >= m_Height) break;
            if (endH > m_Height) {
                endH = m_Height;
            }

            results[i] = std::async(std::launch::async,
                [this, &world, &light, beginW, endW, beginH, endH] {
                    return this->Render(world, light, beginW, endW, beginH, endH);
                });
        }

		std::vector<Color> finalResult(m_Width * m_Height);
		auto resultIt = finalResult.begin();
        for (std::size_t i = 0; i < results.size(); ++i) {
			auto& result = results[i];
            if (!result.valid()) continue;
			auto partialResult = result.get();
            std::copy(partialResult.begin(), partialResult.end(), resultIt);
            resultIt += partialResult.size();
        }
        
		m_Image.SetData(m_Width, m_Height, std::move(finalResult));
    }

    std::vector<Color> Camera::Render(
		const Hittable& world,
        const Hittable& light,
        std::uint32_t beginW, std::uint32_t endW, 
        std::uint32_t beginH, std::uint32_t endH)
    {
        const float invSamplePerPixel = 1.0f / (m_SqrtSPP * m_SqrtSPP);

		std::uint32_t width = endW - beginW;
		std::uint32_t height = endH - beginH;

        std::vector<Color> result(width * height);
		for (std::uint32_t j = beginH; j < endH; j++) {
			for (std::uint32_t i = beginW; i < endW; i++) {
				Color color{};
                for(uint32_t sj = 0; sj < m_SqrtSPP; ++sj){
                    for(uint32_t si = 0; si < m_SqrtSPP; ++si){
                        auto ray = GetRay(i, j, si, sj);
                        color += GetRayColor(ray, m_MaxDepth, world, light);
				    }
                }
					
				color *= invSamplePerPixel;
                result[(j - beginH) * width + (i - beginW)] = color;
			}
		}

        --m_ProgressCount;
        std::clog << "\rScanlines remaining: " << m_ProgressCount << ' ' << std::flush;

        return result;
    }

    void Camera::UpdateCamera()
    {
        m_SqrtSPP = std::sqrt(m_SamplePerPixel);
        m_RecipSqrtSPP = 1.f / m_SqrtSPP;

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

        // 焦散半径
        auto defocusRadius = m_FocusDist * std::tan(DegreesToRadians(m_DefocusAngle / 2));
        m_DefocusDiskU = right * defocusRadius;
        m_DefocusDiskV = -down * defocusRadius;
    }

    // 获取某个像素处的光线
    Ray Camera::GetRay(std::uint32_t x, std::uint32_t y, uint32_t sx, uint32_t sy) const
    {
        auto offset = GetSquareStratified(sx, sy);
        auto pixelSample = m_StartPixelCenter + 
            (float(x) + offset[0]) * m_PixelDeltaU + 
            (float(y) + offset[1]) * m_PixelDeltaV;
        auto rayOrigin = (m_DefocusAngle <= 0) ? m_Pos : DefocusDiskSample();
        float rayTime = RandomFloat();
        return Ray(rayOrigin, (pixelSample - rayOrigin).Normalized(), rayTime);
    }

    // 获取光线采样到的颜色
    Color Camera::GetRayColor(const Ray& ray, int depth, const Hittable& world, const Hittable& light) const
    {
        if (depth <= 0) {
            return Color{0, 0, 0};
        }
        auto interval = Intervalf{0.001f, std::numeric_limits<float>::max()};
        if (auto hitRecord = world.Hit(ray, interval); hitRecord.has_value()) {
            ScatterRecord srec{};
            Color scatter{0,0,0};
            Color emission = hitRecord->m_Material->Emitted(*hitRecord, hitRecord->m_UV, hitRecord->m_Pos);

            if (hitRecord->m_Material->Scatter(ray, hitRecord.value(), srec)) {
                if(srec.m_SkipPDF){
                    return srec.m_Attenuation * GetRayColor(srec.m_SkipPDFRay, depth - 1, world, light);
                }


                auto lightPDF = std::make_shared<HittablePDF>(light, hitRecord->m_Pos);
                MixturePDF pdf{lightPDF, srec.m_PDF};
                Ray scattered = Ray(hitRecord->m_Pos, pdf.Generate(), ray.GetTime());
                float pdfValue = pdf.Value(scattered.GetDirection());
                
                float scatterPDF = hitRecord->m_Material->ScatterPDF(ray, *hitRecord, scattered);

                auto sampleCol = GetRayColor(scattered, depth - 1, world, light);
                scatter = (srec.m_Attenuation * scatterPDF * sampleCol) / pdfValue; // 添加材质
            }
            return scatter + emission;
        }
        
        return m_Background;
    }

    // 获取随机偏移
    Vector2f Camera::GetSquare() const
    {
        return Vector2f{RandomFloat() - 0.5f, RandomFloat() - 0.5f};
    }

    Vector2f Camera::GetSquareStratified(uint32_t sx, uint16_t sy) const
    {
        float x = ((sx + RandomFloat()) * m_RecipSqrtSPP) - .5f;
        float y = ((sy + RandomFloat()) * m_RecipSqrtSPP) - .5f;
        return Vector2f{x, y};
    }

    Vector3f Camera::DefocusDiskSample() const
    {
        // 返回一个在焦散圆盘内的随机向量
        auto p = RandomInUnitDisk();
        return m_Pos + (p[0] * m_DefocusDiskU) + (p[1] * m_DefocusDiskV);
    }
}
