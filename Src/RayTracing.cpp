#include "RayTracing.h"
#include "Material.h"
#include "Pubh.h"
#include "Geometry/Sphere.h"
#include "Geometry/BVH.h"
#include "InstrumentorTimer.h"
#include "Geometry/Quad.h"

namespace DSM {
    RayTracing::RayTracing(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel)
        :m_AspectRatio(aspectRatio), 
        m_Width(width), 
        m_SamplePerPixel(samplePerPixel){
    }
    
    void RayTracing::Render()
    {
        //BuildScene0();
        //BuildScene1();
        //BuildScene2();
        //BuildScene3();
        CornellBox();

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
        auto earthTexture = std::make_shared<ImageTexture>("Textures/earthmap.jpg");
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
                        objs->Add(std::make_shared<Sphere>(center, center1, 0.2f, sphereMaterial));
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

        auto material2 = std::make_shared<LambertMat>(earthTexture);
        objs->Add(std::make_shared<Sphere>(Vector3f{-4, 1, 0}, 1.0, material2));

        auto material3 = std::make_shared<MetalMat>(Color(0.7, 0.6, 0.5), 0.0);
        objs->Add(std::make_shared<Sphere>(Vector3f{4, 1, 0}, 1.0, material3));

        InstrumentationTimer BVHTimer{"Scene0 BVH Build"};        
        objs = std::make_unique<HittableList>(std::make_shared<BVH>(*objs));
        BVHTimer.Stop();

        Camera camera{};
        camera.m_AspectRatio = m_AspectRatio;
        camera.m_Width = m_Width;
        camera.m_SamplePerPixel = m_SamplePerPixel;
        camera.m_MaxDepth = 100;
        camera.m_Vfov = 40;
        camera.m_Lookfrom = {-12, 3, 8};
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

        Camera camera{};

        camera.m_AspectRatio = m_AspectRatio;
        camera.m_Width = m_Width;
        camera.m_SamplePerPixel = m_SamplePerPixel;
        camera.m_MaxDepth = 5;

        camera.m_Vfov = 20;
        camera.m_Lookfrom = Vector3f{13,2,3};
        camera.m_Lookat = Vector3f{0,0,0};
        camera.m_Vup = Vector3f{0,1,0};

        camera.m_DefocusAngle = 0;

        m_Scenes.emplace_back(*world, std::move(camera));
    }

    void RayTracing::BuildScene2()
    {
        auto earthTexture = std::make_shared<ImageTexture>("Textures/earthmap.jpg");
        auto earthMat = std::make_shared<LambertMat>(earthTexture);
        auto globe = std::make_shared<Sphere>(Vector3f{0,0,0}, 2, earthMat);

        Camera camera{};

        camera.m_AspectRatio = m_AspectRatio;
        camera.m_Width = m_Width;
        camera.m_SamplePerPixel = m_SamplePerPixel;
        camera.m_MaxDepth = 50;

        camera.m_Vfov = 20;
        camera.m_Lookfrom = Vector3f{0,0,12};
        camera.m_Lookat = Vector3f{0,0,0};
        camera.m_Vup = Vector3f{0,1,0};

        camera.m_DefocusAngle = 0;

        m_Scenes.emplace_back(HittableList{globe}, std::move(camera));
    }
    
    void RayTracing::BuildScene3()
    {
        auto world = std::make_unique<HittableList>();

        auto pertext = std::make_shared<NoiceTexture>();
        world->Add(std::make_shared<Sphere>(Vector3f{0,-1000,0}, 1000, std::make_shared<LambertMat>(pertext)));
        world->Add(std::make_shared<Sphere>(Vector3f{0,2,0}, 2, std::make_shared<LambertMat>(pertext)));

        Camera camera{};
        camera.m_AspectRatio = m_AspectRatio;
        camera.m_Width = m_Width;
        camera.m_SamplePerPixel = m_SamplePerPixel;
        camera.m_MaxDepth = 50;

        camera.m_Vfov = 20;
        camera.m_Lookfrom = Vector3f{13,2,3};
        camera.m_Lookat = Vector3f{0,0,0};
        camera.m_Vup = Vector3f{0,1,0};

        camera.m_DefocusAngle = 0;

        m_Scenes.emplace_back(*world, std::move(camera));
    }
    
    void RayTracing::CornellBox()
    {
        auto world = std::make_unique<HittableList>();

        auto red   = std::make_shared<LambertMat>(Color(.65, .05, .05));
        auto white = std::make_shared<LambertMat>(Color(.73, .73, .73));
        auto green = std::make_shared<LambertMat>(Color(.12, .45, .15));

        world->Add(std::make_shared<Quad>(Vector3f{555,0,0}, Vector3f{0,555,0}, Vector3f{0,0,555}, green));
        world->Add(std::make_shared<Quad>(Vector3f{0,0,0}, Vector3f{0,555,0}, Vector3f{0,0,555}, red));
        world->Add(std::make_shared<Quad>(Vector3f{0,0,0}, Vector3f{555,0,0}, Vector3f{0,0,555}, white));
        world->Add(std::make_shared<Quad>(Vector3f{555,555,555}, Vector3f{-555,0,0}, Vector3f{0,0,-555}, white));
        world->Add(std::make_shared<Quad>(Vector3f{0,0,555}, Vector3f{555,0,0}, Vector3f{0,555,0}, white));

        Camera camera{};

        camera.m_AspectRatio = m_AspectRatio;
        camera.m_Width = m_Width;
        camera.m_SamplePerPixel = m_SamplePerPixel;
        camera.m_MaxDepth = 50;

        camera.m_Vfov = 40;
        camera.m_Lookfrom = Vector3f{278, 278, -800};
        camera.m_Lookat = Vector3f{278, 278, 0};
        camera.m_Vup = Vector3f{0,1,0};

        camera.m_DefocusAngle = 0;

        m_Scenes.emplace_back(*world, std::move(camera));
    }
}
