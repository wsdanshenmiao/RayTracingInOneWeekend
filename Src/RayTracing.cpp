#include "RayTracing.h"
#include "Material.h"
#include "Pubh.h"
#include "Geometry/Sphere.h"
#include "Geometry/BVH.h"
#include "InstrumentorTimer.h"
#include "Geometry/Quad.h"
#include "Geometry/ConstantMedium.h"
#include <chrono>
#include <format>
#include <string>
#include <print>


namespace DSM {
    RayTracing::RayTracing(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel,
                           int maxDepth, bool useMultithread)
        :m_AspectRatio(aspectRatio), 
        m_Width(width), 
        m_SamplePerPixel(samplePerPixel),
        m_MaxDepth(maxDepth),
        m_UseMultithread(useMultithread){
    }
    
    void RayTracing::Render()
    {
        //BuildScene0();
        //BuildScene1();
        //BuildScene2();
        //BuildScene3();
        CornellBox();
        //SampleLight();
        //CornellSmoke();
        //FinalScene();

        // 把命令行传入的渲染设置下发到所有场景
        ApplyRenderSettings();

        m_SceneTimes.clear();
        m_SceneTimes.reserve(m_Scenes.size());

        // 逐场景渲染，统计各自的光线追踪耗时
        for(std::size_t i = 0; i < m_Scenes.size(); ++i){
            std::string timerName{"RayTracingTimer" + std::to_string(i)};
            InstrumentationTimer RayTracingTimer{timerName.c_str()};

            const auto startTime = std::chrono::steady_clock::now();
            m_Scenes[i]->Render();
            const auto endTime = std::chrono::steady_clock::now();

            RayTracingTimer.Stop();

            m_SceneTimes.push_back(
                std::chrono::duration<double, std::milli>(endTime - startTime).count());
        }

        PrintTimingSummary();
    }

    void RayTracing::ApplyRenderSettings()
    {
        for (auto& scene : m_Scenes) {
            // 仅当显式指定了正数深度时才覆盖场景内置深度
            if (m_MaxDepth > 0) {
                scene->SetMaxDepth(m_MaxDepth);
            }
            scene->SetUseMultithread(m_UseMultithread);
        }
    }

    void RayTracing::PrintTimingSummary() const
    {
        constexpr std::string_view line =
            "-------------------------------------------------------------------";

        double totalMs = 0.0;
        for (double ms : m_SceneTimes) {
            totalMs += ms;
        }

        std::println("");
        std::println("==================== Ray Tracing Timing Report ====================");
        std::println("  Resolution      : {} x {}", m_Width,
                     static_cast<std::uint32_t>(m_Width / m_AspectRatio));
        std::println("  Samples / pixel : {}", m_SamplePerPixel);
        std::println("  Max depth       : {}",
                     m_MaxDepth > 0 ? std::to_string(m_MaxDepth) : std::string{"per-scene"});
        std::println("  Multithreading  : {}",
                     m_UseMultithread ? "ON" : "OFF");
        std::println("  Scene count     : {}", m_Scenes.size());
        std::println("{}", line);

        for (std::size_t i = 0; i < m_Scenes.size(); ++i) {
            const double ms = (i < m_SceneTimes.size()) ? m_SceneTimes[i] : 0.0;
            const std::string label =
                std::format("Scene {} [{}]", i, m_Scenes[i]->GetName());
            std::println("  {:<33}{:>12.3f} ms", label, ms);
        }

        std::println("{}", line);
        std::println("  {:<33}{:>12.3f} ms", std::string{"Total"}, totalMs);
        std::println("===================================================================");
    }

    const Scene &RayTracing::GetScene(std::size_t index) const
    {
        return *m_Scenes[index];
    }

    void RayTracing::BuildScene0()
    {
        auto checkTexture = std::make_shared<CheckerTexture>(
            TextureDesc{10, 10, 0}, 0.02f, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
        auto earthTexture = std::make_shared<ImageTexture>("Textures/earthmap.jpg");
        auto ground_material = std::make_shared<LambertMat>(checkTexture);
        auto objs = std::make_shared<HittableList>();
        objs->Add(std::make_shared<Sphere>(Vector3f{0, -1000, 0}, 1000.f, ground_material));

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
                        objs->Add(std::make_shared<Sphere>(center, 0.2f, sphereMaterial));
                    }
                    else {
                        // glass
                        sphereMaterial = std::make_shared<DielectricMat>(1.5f);
                        objs->Add(std::make_shared<Sphere>(center, 0.2f, sphereMaterial));
                    }
                }
            }
        }

        auto material1 = std::make_shared<DielectricMat>(1.5f);
        objs->Add(std::make_shared<Sphere>(Vector3f{0, 1, 0}, 1.0f, material1));

        auto material2 = std::make_shared<LambertMat>(earthTexture);
        objs->Add(std::make_shared<Sphere>(Vector3f{-4, 1, 0}, 1.0f, material2));

        auto material3 = std::make_shared<MetalMat>(Color(0.7, 0.6, 0.5), 0.0f);
        objs->Add(std::make_shared<Sphere>(Vector3f{4, 1, 0}, 1.0f, material3));

        InstrumentationTimer BVHTimer{"Scene0 BVH Build"};        
        objs = std::make_shared<HittableList>(std::make_shared<BVH>(*objs));
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
        camera.m_Background = Color{0.70, 0.80, 1.00};

        m_Scenes.emplace_back(std::make_unique<Scene>(objs, nullptr, std::move(camera), "Random Spheres"));
    }

    void RayTracing::BuildScene1()
    {
        auto world = std::make_shared<HittableList>();

        auto checker = std::make_shared<CheckerTexture>(
            TextureDesc{10, 10, 1}, .2f, Color(.2, .3, .1), Color(.9, .9, .9));

        world->Add(std::make_shared<Sphere>(Vector3f{0,-10, 0}, 10.0f, std::make_shared<LambertMat>(checker)));
        world->Add(std::make_shared<Sphere>(Vector3f{0, 10, 0}, 10.0f, std::make_shared<LambertMat>(checker)));

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

        m_Scenes.emplace_back(std::make_unique<Scene>(world, nullptr, std::move(camera), "Checkered Spheres"));
    }

    void RayTracing::BuildScene2()
    {
        auto earthTexture = std::make_shared<ImageTexture>("Textures/earthmap.jpg");
        auto earthMat = std::make_shared<LambertMat>(earthTexture);
        auto globe = std::make_shared<Sphere>(Vector3f{0,0,0}, 2.0f, earthMat);

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

        m_Scenes.emplace_back(std::make_unique<Scene>(globe, nullptr, std::move(camera), "Earth"));
    }
    
    void RayTracing::BuildScene3()
    {
        auto world = std::make_shared<HittableList>();

        auto pertext = std::make_shared<NoiceTexture>();
        world->Add(std::make_shared<Sphere>(Vector3f{0,-1000,0}, 1000.0f, std::make_shared<LambertMat>(pertext)));
        world->Add(std::make_shared<Sphere>(Vector3f{0,2,0}, 2.0f, std::make_shared<LambertMat>(pertext)));

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
        camera.m_Background = Color{0.70, 0.80, 1.00};

        m_Scenes.emplace_back(std::make_unique<Scene>(world, nullptr, std::move(camera), "Perlin Spheres"));
    }
    
    void RayTracing::CornellBox()
    {
        auto world = std::make_shared<HittableList>();

        auto red   = std::make_shared<LambertMat>(Color(.65, .05, .05));
        auto white = std::make_shared<LambertMat>(Color(.73, .73, .73));
        auto green = std::make_shared<LambertMat>(Color(.12, .45, .15));
        auto lightMat = std::make_shared<DiffuseLightMat>(Color(15, 15, 15));

        auto light = std::make_shared<Quad>(Vector3f{343, 554, 332}, Vector3f{-130,0,0}, Vector3f{0,0,-105}, lightMat);

        world->Add(std::make_shared<Quad>(Vector3f{555,0,0}, Vector3f{0,555,0}, Vector3f{0,0,555}, green));
        world->Add(std::make_shared<Quad>(Vector3f{0,0,0}, Vector3f{0,555,0}, Vector3f{0,0,555}, red));
        world->Add(std::make_shared<Quad>(Vector3f{0,0,0}, Vector3f{555,0,0}, Vector3f{0,0,555}, white));
        world->Add(std::make_shared<Quad>(Vector3f{555,555,555}, Vector3f{-555,0,0}, Vector3f{0,0,-555}, white));
        world->Add(std::make_shared<Quad>(Vector3f{0,0,555}, Vector3f{555,0,0}, Vector3f{0,555,0}, white));
        world->Add(light);



        auto aluminum = std::make_shared<MetalMat>(Color{0.8, 0.85, 0.88}, 0.0f);
        std::shared_ptr<Hittable> box1 = Geometry::Box(Vector3f{0,0,0}, Vector3f{165,330,165}, white);
        box1 = std::make_shared<RotateY>(box1, 15.f);
        box1 = std::make_shared<Translate>(box1, Vector3f{265,0,295});
        world->Add(box1);

        auto glass = std::make_shared<DielectricMat>(1.5f);
        world->Add(std::make_shared<Sphere>(Vector3f{190,90,190}, 90.f, glass));

        // std::shared_ptr<Hittable> box2 = Geometry::Box(Vector3f{0,0,0}, Vector3f{165,165,165}, white);
        // box2 = std::make_shared<RotateY>(box2, -18);
        // box2 = std::make_shared<Translate>(box2, Vector3f{130,0,65});
        // world->Add(box2);

        //world->Add(Geometry::Box(Vector3f{130, 0, 65}, Vector3f{295, 165, 230}, white));
        //world->Add(Geometry::Box(Vector3f{265, 0, 295}, Vector3f{430, 330, 460}, white));

        //world = std::make_shared<HittableList>(std::make_shared<BVH>(*world));

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
        camera.m_Background = Color{0,0,0};

        m_Scenes.emplace_back(std::make_unique<Scene>(world, light, std::move(camera), "Cornell Box"));
    }
    
    void RayTracing::SampleLight()
    {
        auto world = std::make_shared<HittableList>();

        auto earthTexture = std::make_shared<ImageTexture>("Textures/earthmap.jpg");
        world->Add(std::make_shared<Sphere>(Vector3f{0,-1000,0}, 1000.0f, std::make_shared<LambertMat>(Color{0.5, 0.5, 0.5})));
        world->Add(std::make_shared<Sphere>(Vector3f{0,2,0}, 2.0f, std::make_shared<LambertMat>(earthTexture)));

        auto difflight = std::make_shared<DiffuseLightMat>(Color(4,4,4));

        auto sphereLight = std::make_shared<Sphere>(Vector3f{0,7,0}, 2.0f, difflight);
        auto quadLight = std::make_shared<Quad>(Vector3f{3,1,-2}, Vector3f{2,0,0}, Vector3f{0,2,0}, difflight);
        auto lights = std::make_shared<HittableList>(std::vector<std::shared_ptr<Hittable>>{sphereLight, quadLight});

        world->Add(lights);

        Camera camera{};

        camera.m_AspectRatio = m_AspectRatio;
        camera.m_Width = m_Width;
        camera.m_SamplePerPixel = m_SamplePerPixel;
        camera.m_MaxDepth = 50;

        camera.m_Vfov = 20;
        camera.m_Lookfrom = Vector3f{26,3,6};
        camera.m_Lookat = Vector3f{0,2,0};
        camera.m_Vup = Vector3f{0,1,0};

        camera.m_DefocusAngle = 0;
        camera.m_Background = Color{0, 0, 0};

        m_Scenes.emplace_back(std::make_unique<Scene>(world, lights, std::move(camera), "Light Sampling"));
    }
    
    void RayTracing::CornellSmoke()
    {
        auto world = std::make_shared<HittableList>();

        auto earthTexture = std::make_shared<ImageTexture>("Textures/earthmap.jpg");
        auto red   = std::make_shared<LambertMat>(Color(.65, .05, .05));
        auto white = std::make_shared<LambertMat>(Color(.73, .73, .73));
        auto green = std::make_shared<LambertMat>(Color(.12, .45, .15));
        auto lightMat = std::make_shared<DiffuseLightMat>(Color(7, 7, 7));

        auto light = std::make_shared<Quad>(Vector3f{113,554,127}, Vector3f{330,0,0}, Vector3f{0,0,305}, lightMat);

        world->Add(std::make_shared<Quad>(Vector3f{555,0,0}, Vector3f{0,555,0}, Vector3f{0,0,555}, green));
        world->Add(std::make_shared<Quad>(Vector3f{0,0,0}, Vector3f{0,555,0}, Vector3f{0,0,555}, red));
        world->Add(std::make_shared<Quad>(Vector3f{0,0,0}, Vector3f{555,0,0}, Vector3f{0,0,555}, white));
        world->Add(std::make_shared<Quad>(Vector3f{555,555,555}, Vector3f{-555,0,0}, Vector3f{0,0,-555}, white));
        world->Add(std::make_shared<Quad>(Vector3f{0,555,555}, Vector3f{555,0,0}, Vector3f{0,-555,0}, 
            std::make_shared<LambertMat>(earthTexture)));
        world->Add(light);


        std::shared_ptr<Hittable> box1 = Geometry::Box(Vector3f{0,0,0}, Vector3f{165,330,165}, white);
        box1 = std::make_shared<RotateY>(box1, 15.f);
        box1 = std::make_shared<Translate>(box1, Vector3f{265,0,295});

        std::shared_ptr<Hittable> box2 = Geometry::Box(Vector3f{0,0,0}, Vector3f{165,165,165}, white);
        box2 = std::make_shared<RotateY>(box2, -18.0f);
        box2 = std::make_shared<Translate>(box2, Vector3f{130,0,65});

        world->Add(std::make_shared<ConstantMedium>(box1, 0.01f, Color{0,0,0}));
        world->Add(std::make_shared<ConstantMedium>(box2, 0.01f, Color{1,1,1}));

        //world = std::make_shared<HittableList>(std::make_shared<BVH>(*world));

        Camera camera{};

        camera.m_AspectRatio = m_AspectRatio;
        camera.m_Width = m_Width;
        camera.m_SamplePerPixel = m_SamplePerPixel;
        camera.m_MaxDepth = 100;

        camera.m_Vfov = 40;
        camera.m_Lookfrom = Vector3f{278, 278, -800};
        camera.m_Lookat = Vector3f{278, 278, 0};
        camera.m_Vup = Vector3f{0,1,0};

        camera.m_DefocusAngle = 0;
        camera.m_Background = Color{0,0,0};

        m_Scenes.emplace_back(std::make_unique<Scene>(world, light, std::move(camera), "Cornell Smoke"));
    }

    void RayTracing::FinalScene()
    {
        auto boxes1 = std::make_shared<HittableList>();
        auto ground = std::make_shared<LambertMat>(Color(0.48, 0.83, 0.53));

        int boxes_per_side = 20;
        for (int i = 0; i < boxes_per_side; i++) {
            for (int j = 0; j < boxes_per_side; j++) {
                float w = 100.0;
                float x0 = -1000.0 + i*w;
                float z0 = -1000.0 + j*w;
                float y0 = 0.0;
                float x1 = x0 + w;
                float y1 = RandomFloat(1,101);
                float z1 = z0 + w;

                boxes1->Add(Geometry::Box(Vector3f{x0,y0,z0}, Vector3f{x1,y1,z1}, ground));
            }
        }

        auto world = std::make_shared<HittableList>();

        world->Add(std::make_shared<BVH>(*boxes1));

        auto lightMat = std::make_shared<DiffuseLightMat>(Color(7, 7, 7));
        auto light = std::make_shared<Quad>(Vector3f{123,554,147}, Vector3f{300,0,0}, Vector3f{0,0,265}, lightMat);
        world->Add(light);

        auto center1 = Vector3f{400, 400, 200};
        auto center2 = center1 + Vector3f{30,0,0};
        auto sphere_material = std::make_shared<LambertMat>(Color(0.7, 0.3, 0.1));
        world->Add(std::make_shared<Sphere>(center1, center2, 50.0f, sphere_material));

        world->Add(std::make_shared<Sphere>(Vector3f{260, 150, 45}, 50.0f, std::make_shared<DielectricMat>(1.5f)));
        world->Add(std::make_shared<Sphere>(
            Vector3f{0, 150, 145}, 50.0f, std::make_shared<MetalMat>(Color(0.8, 0.8, 0.9), 1.0f)
        ));

        auto boundary = std::make_shared<Sphere>(Vector3f{360,150,145}, 70.0f, std::make_shared<DielectricMat>(1.5f));
        world->Add(boundary);
        world->Add(std::make_shared<ConstantMedium>(boundary, 0.2f, Color(0.2, 0.4, 0.9)));
        boundary = std::make_shared<Sphere>(Vector3f{0,0,0}, 5000.0f, std::make_shared<DielectricMat>(1.5f));
        world->Add(std::make_shared<ConstantMedium>(boundary, .0001f, Color(1,1,1)));

        auto emat = std::make_shared<LambertMat>(std::make_shared<ImageTexture>("Textures/earthmap.jpg"));
        world->Add(std::make_shared<Sphere>(Vector3f{400,200,400}, 100.0f, emat));
        auto pertext = std::make_shared<NoiceTexture>(0.4f);
        world->Add(std::make_shared<Sphere>(Vector3f{220,280,300}, 80.0f, std::make_shared<LambertMat>(pertext)));

        auto boxes2 = std::make_shared<HittableList>();
        auto white = std::make_shared<LambertMat>(Color(.73, .73, .73));
        int ns = 1000;
        for (int j = 0; j < ns; j++) {
            boxes2->Add(std::make_shared<Sphere>(RandomVector3f(0,165), 10.0f, white));
        }

        world->Add(std::make_shared<Translate>(
            std::make_shared<RotateY>(
                std::make_shared<BVH>(*boxes2), 15.f),
                Vector3f{-100,270,395}
            )
        );

        Camera camera{};

        camera.m_AspectRatio = m_AspectRatio;
        camera.m_Width = m_Width;
        camera.m_SamplePerPixel = m_SamplePerPixel;
        camera.m_MaxDepth = 50;
        camera.m_Background = Color(0,0,0);

        camera.m_Vfov = 40;
        camera.m_Lookfrom = Vector3f{478, 278, -600};
        camera.m_Lookat = Vector3f{278, 278, 0};
        camera.m_Vup = Vector3f{0,1,0};

        camera.m_DefocusAngle = 0;

        m_Scenes.emplace_back(std::make_unique<Scene>(world, light, std::move(camera), "Final Scene"));
    }
}
