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
        m_SamplePerPixel(samplePerPixel){
    }
    
    void RayTracing::Render()
    {
        BuildScene0();
        BuildScene1();

        for(std::size_t i = 0; i < m_Scenes.size(); ++i){
            std::string timerName{"RayTracingTimer" + std::to_string(i)};
            InstrumentationTimer RayTracingTimer{timerName.c_str()};
            m_Scenes[i].Render();
            RayTracingTimer.Stop();
        }
    }

    const Scene &RayTracing::GetScene(std::size_t index) const
    {
        return m_Scenes[index];
    }

    void RayTracing::BuildScene0()
    {
        auto checkTexture = std::make_shared<CheckerTexture>(
            TextureDesc{10, 10, 0}, 0.02f, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
        auto ground_material = std::make_shared<LambertMat>(checkTexture);
        auto objs = std::make_unique<HittableList>();
        objs->Add(std::make_shared<Sphere>(Vector3f{0, -1000, 0}, 1000, ground_material));

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
                        objs->Add(std::make_shared<Sphere>(center, center, 0.2f, sphereMaterial));
                    }
                    else if (choose_mat < 0.95) {
                        // metal
                        auto albedo = randomColor;
                        auto fuzz = RandomFloat(0, 0.5);
                        sphereMaterial = std::make_shared<MetalMat>(albedo, fuzz);
                        objs->Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                    }
                    else {
                        // glass
                        sphereMaterial = std::make_shared<DielectricMat>(1.5);
                        objs->Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                    }
                }
            }
        }

        auto material1 = std::make_shared<DielectricMat>(1.5);
        objs->Add(std::make_shared<Sphere>(Vector3f{0, 1, 0}, 1.0, material1));

        auto material2 = std::make_shared<LambertMat>(Color{0.4, 0.2, 0.1});
        objs->Add(std::make_shared<Sphere>(Vector3f{-4, 1, 0}, 1.0, material2));

        auto material3 = std::make_shared<MetalMat>(Color(0.7, 0.6, 0.5), 0.0);
        objs->Add(std::make_shared<Sphere>(Vector3f{4, 1, 0}, 1.0, material3));

        InstrumentationTimer BVHTimer{"Scene0 BVH Build"};        
        objs = std::make_unique<HittableList>(std::make_shared<BVH>(*objs));
        BVHTimer.Stop();

        Camera camera;
        camera.m_MaxDepth = 20;
        camera.m_Vfov = 40;
        camera.m_Lookfrom = {15, 3, 5};
        camera.m_Lookat = {0, 0, 0};
        camera.m_Vup = {0, 1, 0};
        camera.m_DefocusAngle = 0.6f;
        camera.m_FocusDist = 10.0;

        m_Scenes.emplace_back(*objs, std::move(camera));
    }

    void RayTracing::BuildScene1()
    {
        std::unique_ptr<HittableList> world = std::make_unique<HittableList>();

        auto checker = std::make_shared<CheckerTexture>(
            TextureDesc{10, 10, 1}, .2f, Color(.2, .3, .1), Color(.9, .9, .9));

        world->Add(std::make_shared<Sphere>(Vector3f{0,-10, 0}, 10, std::make_shared<LambertMat>(checker)));
        world->Add(std::make_shared<Sphere>(Vector3f{0, 10, 0}, 10, std::make_shared<LambertMat>(checker)));

        Camera cam;

        cam.m_AspectRatio = 16.0 / 9.0;
        cam.m_Width = 400;
        cam.m_SamplePerPixel = 10;
        cam.m_MaxDepth = 5;

        cam.m_Vfov = 20;
        cam.m_Lookfrom = Vector3f{13,2,3};
        cam.m_Lookat = Vector3f{0,0,0};
        cam.m_Vup = Vector3f{0,1,0};

        cam.m_DefocusAngle = 0;

        m_Scenes.emplace_back(*world, std::move(cam));
    }
}
