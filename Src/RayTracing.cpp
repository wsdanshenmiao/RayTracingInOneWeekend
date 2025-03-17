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
        auto materialLeft   = std::make_shared<MetalMat>(Color{0.8, 0.8, 0.8}, 1);
        auto materialRight  = std::make_shared<MetalMat>(Color{0.8, 0.6, 0.2}, 0.3);

        m_World->Add(std::make_shared<Sphere>(Vector3f{0.0, -100.5, 1.0}, 100.0, materialGround));
        m_World->Add(std::make_shared<Sphere>(Vector3f{0.0, 0.0, 1.2},   0.5, materialCenter));
        m_World->Add(std::make_shared<Sphere>(Vector3f{-1.0, 0.0, 1.0},   0.5, materialLeft));
        m_World->Add(std::make_shared<Sphere>(Vector3f{1.0, 0.0, 1.0},   0.5, materialRight));
        
        camera.Render(*m_World);
    }
}
