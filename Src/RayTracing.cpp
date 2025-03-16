#include "RayTracing.h"

#include "Camera.h"
#include "Pubh.h"
#include "Geometry/Sphere.h"

namespace DSM {
    RayTracing::RayTracing(float aspectRatio, int width)
        :m_AspectRatio(aspectRatio), m_Width(width), m_World(std::make_unique<HittableList>()){
    }

    void RayTracing::Render() const
    {
        Camera camera{m_AspectRatio, m_Width};
        
        m_World->Add(std::make_shared<Sphere>(Vector3f{0,0,1}, 0.5));
        m_World->Add(std::make_shared<Sphere>(Vector3f{0,-100.5,1}, 100));
        
        camera.Render(*m_World);
    }
}
