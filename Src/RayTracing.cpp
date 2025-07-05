#include "RayTracing.h"
#include "Material.h"
#include "Pubh.h"
#include "Geometry/Sphere.h"
#include "Geometry/BVH.h"
#include "InstrumentorTimer.h"
#include "Geometry/Quad.h"
#include "Geometry/ConstantMedium.h"
#include <print>


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
        //SampleLight();
        //CornellSmoke();
        //FinalScene();

        for(std::size_t i = 0; i < m_Scenes.size(); ++i){
            std::string timerName{"RayTracingTimer" + std::to_string(i)};
            InstrumentationTimer RayTracingTimer{timerName.c_str()};
            m_Scenes[i]->Render();
            RayTracingTimer.Stop();
        }
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

        m_Scenes.emplace_back(std::make_unique<Scene>(objs, nullptr, std::move(camera)));
    }

    void RayTracing::BuildScene1()
    {
        auto world = std::make_shared<HittableList>();

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

        m_Scenes.emplace_back(std::make_unique<Scene>(world, nullptr, std::move(camera)));
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

        m_Scenes.emplace_back(std::make_unique<Scene>(globe, nullptr, std::move(camera)));
    }
    
    void RayTracing::BuildScene3()
    {
        auto world = std::make_shared<HittableList>();

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
        camera.m_Background = Color{0.70, 0.80, 1.00};

        m_Scenes.emplace_back(std::make_unique<Scene>(world, nullptr, std::move(camera)));
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


        auto aluminum = std::make_shared<MetalMat>(Color{0.8, 0.85, 0.88}, 0.0);
        std::shared_ptr<Hittable> box1 = Geometry::Box(Vector3f{0,0,0}, Vector3f{165,330,165}, aluminum);
        box1 = std::make_shared<RotateY>(box1, 15);
        box1 = std::make_shared<Translate>(box1, Vector3f{265,0,295});
        world->Add(box1);

        std::shared_ptr<Hittable> box2 = Geometry::Box(Vector3f{0,0,0}, Vector3f{165,165,165}, white);
        box2 = std::make_shared<RotateY>(box2, -18);
        box2 = std::make_shared<Translate>(box2, Vector3f{130,0,65});
        world->Add(box2);

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

        m_Scenes.emplace_back(std::make_unique<Scene>(world, light, std::move(camera)));
    }
    
    void RayTracing::SampleLight()
    {
        auto world = std::make_shared<HittableList>();

        auto earthTexture = std::make_shared<ImageTexture>("Textures/earthmap.jpg");
        world->Add(std::make_shared<Sphere>(Vector3f{0,-1000,0}, 1000, std::make_shared<LambertMat>(Color{0.5, 0.5, 0.5})));
        world->Add(std::make_shared<Sphere>(Vector3f{0,2,0}, 2, std::make_shared<LambertMat>(earthTexture)));

        auto difflight = std::make_shared<DiffuseLightMat>(Color(4,4,4));

        auto sphereLight = std::make_shared<Sphere>(Vector3f{0,7,0}, 2, difflight);
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

        m_Scenes.emplace_back(std::make_unique<Scene>(world, lights, std::move(camera)));
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
        box1 = std::make_shared<RotateY>(box1, 15);
        box1 = std::make_shared<Translate>(box1, Vector3f{265,0,295});

        std::shared_ptr<Hittable> box2 = Geometry::Box(Vector3f{0,0,0}, Vector3f{165,165,165}, white);
        box2 = std::make_shared<RotateY>(box2, -18);
        box2 = std::make_shared<Translate>(box2, Vector3f{130,0,65});

        world->Add(std::make_shared<ConstantMedium>(box1, 0.01, Color{0,0,0}));
        world->Add(std::make_shared<ConstantMedium>(box2, 0.01, Color{1,1,1}));

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

        m_Scenes.emplace_back(std::make_unique<Scene>(world, light, std::move(camera)));
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
        world->Add(std::make_shared<Sphere>(center1, center2, 50, sphere_material));

        world->Add(std::make_shared<Sphere>(Vector3f{260, 150, 45}, 50, std::make_shared<DielectricMat>(1.5)));
        world->Add(std::make_shared<Sphere>(
            Vector3f{0, 150, 145}, 50, std::make_shared<MetalMat>(Color(0.8, 0.8, 0.9), 1.0)
        ));

        auto boundary = std::make_shared<Sphere>(Vector3f{360,150,145}, 70, std::make_shared<DielectricMat>(1.5));
        world->Add(boundary);
        world->Add(std::make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
        boundary = std::make_shared<Sphere>(Vector3f{0,0,0}, 5000, std::make_shared<DielectricMat>(1.5));
        world->Add(std::make_shared<ConstantMedium>(boundary, .0001, Color(1,1,1)));

        auto emat = std::make_shared<LambertMat>(std::make_shared<ImageTexture>("Textures/earthmap.jpg"));
        world->Add(std::make_shared<Sphere>(Vector3f{400,200,400}, 100, emat));
        auto pertext = std::make_shared<NoiceTexture>(0.4);
        world->Add(std::make_shared<Sphere>(Vector3f{220,280,300}, 80, std::make_shared<LambertMat>(pertext)));

        auto boxes2 = std::make_shared<HittableList>();
        auto white = std::make_shared<LambertMat>(Color(.73, .73, .73));
        int ns = 1000;
        for (int j = 0; j < ns; j++) {
            boxes2->Add(std::make_shared<Sphere>(RandomVector3f(0,165), 10, white));
        }

        world->Add(std::make_shared<Translate>(
            std::make_shared<RotateY>(
                std::make_shared<BVH>(*boxes2), 15),
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

        m_Scenes.emplace_back(std::make_unique<Scene>(world, light, std::move(camera)));
    }
}
