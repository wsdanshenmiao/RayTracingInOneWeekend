#include "RayTracing.h"
#include "Material.h"
#include "Pubh.h"
#include "Geometry/Sphere.h"
#include "Geometry/BVH.h"
#include "InstrumentorTimer.h"

namespace DSM {
    RayTracing::RayTracing(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel)
        :m_AspectRatio(aspectRatio), 
        m_Width(width), 
        m_SamplePerPixel(samplePerPixel), 
        m_World(std::make_unique<HittableList>()),
        m_Camera(m_AspectRatio, m_Width, m_SamplePerPixel){
    }
    
    const Image& RayTracing::Render()
    {
        auto ground_material = std::make_shared<LambertMat>(Color(0.5, 0.5, 0.5));
        m_World->Add(std::make_shared<Sphere>(Vector3f{0, -1000, 0}, 1000, ground_material));

        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                auto choose_mat = RandomFloat();
                Vector3f center{a + 0.9f * RandomFloat(), 0.2f, b + 0.9f * RandomFloat()};

                if ((center - Vector3f{4, 0.2, 0}).Magnitude() > 0.9) {
                    std::shared_ptr<Material> sphereMaterial;

                    auto randomColor =  Color{RandomFloat(0, 1), RandomFloat(0, 1), RandomFloat(0, 1)};
                    if (choose_mat < 0.8) {
                        // diffuse
                        auto albedo = randomColor;
                        sphereMaterial = std::make_shared<LambertMat>(albedo);
						Vector3f center1 = center + Vector3f{ 0, RandomFloat(0, 0.5), 0 };
                        m_World->Add(std::make_shared<Sphere>(center, center, 0.2f, sphereMaterial));
                    }
                    else if (choose_mat < 0.95) {
                        // metal
                        auto albedo = randomColor;
                        auto fuzz = RandomFloat(0, 0.5);
                        sphereMaterial = std::make_shared<MetalMat>(albedo, fuzz);
                        m_World->Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                    }
                    else {
                        // glass
                        sphereMaterial = std::make_shared<DielectricMat>(1.5);
                        m_World->Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                    }
                }
            }
        }

        auto material1 = std::make_shared<DielectricMat>(1.5);
        m_World->Add(std::make_shared<Sphere>(Vector3f{0, 1, 0}, 1.0, material1));

        auto material2 = std::make_shared<LambertMat>(Color{0.4, 0.2, 0.1});
        m_World->Add(std::make_shared<Sphere>(Vector3f{-4, 1, 0}, 1.0, material2));

        auto material3 = std::make_shared<MetalMat>(Color(0.7, 0.6, 0.5), 0.0);
        m_World->Add(std::make_shared<Sphere>(Vector3f{4, 1, 0}, 1.0, material3));

        InstrumentationTimer BVHTimer{"BVH Build"};        
        m_World = std::make_unique<HittableList>(std::make_shared<BVH>(*m_World));
        BVHTimer.Stop();

        m_Camera.m_MaxDepth = 20;

        m_Camera.m_Vfov = 20;
        m_Camera.m_Lookfrom = {13, 2, 3};
        m_Camera.m_Lookat = {0, 0, 0};
        m_Camera.m_Vup = {0, 1, 0};

        m_Camera.m_DefocusAngle = 0.6f;
        m_Camera.m_FocusDist = 10.0;
        
        InstrumentationTimer RayTracingTimer{"RayTracingTimer"};
        return m_Camera.Render(*m_World);
    }
}
