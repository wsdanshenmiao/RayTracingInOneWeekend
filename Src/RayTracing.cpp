#include "RayTracing.h"
#include "Camera.h"
#include "Material.h"
#include "Pubh.h"
#include "Geometry/Sphere.h"

namespace DSM {
    RayTracing::RayTracing(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel)
        :m_AspectRatio(aspectRatio), m_Width(width), m_SamplePerPixel(samplePerPixel), m_World(std::make_unique<HittableList>()){
    }
    
    void RayTracing::Render() const
    {
        Camera camera{m_AspectRatio, m_Width, m_SamplePerPixel};
        
        auto materialGround = std::make_shared<LambertMat>(Color{0.8, 0.8, 0.0});
        auto materialCenter = std::make_shared<LambertMat>(Color{0.1, 0.2, 0.5});
        auto materialRight = std::make_shared<MetalMat>(Color{0.8, 0.8, 0.8}, 0.3f);
        auto materialLeft = std::make_shared<DielectricMat>(1.50);
        auto materialBbubble = std::make_shared<DielectricMat>(1.00 / 1.50);

        m_World->Add(std::make_shared<Sphere>(Vector3f{0.0, -100.5, 1.0}, 100.0, materialGround));
        m_World->Add(std::make_shared<Sphere>(Vector3f{0.0, 0.0, 1.2},   0.5, materialCenter));
        m_World->Add(std::make_shared<Sphere>(Vector3f{-1.0, 0.0, 1.0},   0.5, materialLeft));
        m_World->Add(std::make_shared<Sphere>(Vector3f{1.0, 0.0, 1.0},   0.5, materialRight));
        m_World->Add(std::make_shared<Sphere>(Vector3f{-1.0, 0.0, 1.0},   0.4, materialBbubble));
        camera.m_Lookfrom = {-2,2,-1};
        camera.m_Lookat = {0,0,1};
        camera.m_Vup = {0,1,0};
        camera.m_Vfov = 20;
        camera.m_DefocusAngle = 10;
        camera.m_FocusDist = 3.4f;
        
        camera.Render(*m_World);
    }
}
