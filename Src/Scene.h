#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include "Geometry/HittableList.h"
#include "Camera.h"
#include <mutex>
#include <string>

namespace DSM {

    // Render 与场景/相机修改在场景级串行执行，无需在每次光线查询时加锁。
    // 调用方也不得通过外部别名修改正在渲染的几何体、材质或纹理。
    class Scene {
    public:
        Scene(std::shared_ptr<Hittable> world, std::shared_ptr<Hittable> light, Camera camera,
              std::string name = "Scene")
            : m_World(world), m_Light(light), m_Camera(std::move(camera)), m_Name(std::move(name)) {}
        Scene(std::vector<std::shared_ptr<Hittable>> objects, 
            std::vector<std::shared_ptr<Hittable>> light,
            Camera camera, std::string name = "Scene")
            :m_World(std::make_shared<HittableList>(std::move(objects))), 
            m_Light(std::make_shared<HittableList>(std::move(light))),
            m_Camera(std::move(camera)),
            m_Name(std::move(name)) {}
        
        void AddObject(std::shared_ptr<Hittable> object);
        void AddObjects(const std::vector<std::shared_ptr<Hittable>>& objects);
        void Clear();

        void Render();
        // 返回借用引用；仅在 Render 完成且没有并发修改时读取。
        const Image& GetCameraImage() const { return m_Camera.GetImage(); }

        void SetCamera(Camera camera);

        // 场景名称（用于渲染耗时报告）
        const std::string& GetName() const noexcept { return m_Name; }

        // 渲染设置转发到内部相机
        void SetMaxDepth(int depth);
        void SetUseMultithread(bool useMultithread);
        // 与 GetCameraImage 相同，调用方须保证读取期间没有并发修改。
        const Camera& GetCamera() const noexcept { return m_Camera; }

    private:
        std::mutex m_Mutex{};
        std::vector<std::shared_ptr<Hittable>> m_ObjCache;

        std::shared_ptr<Hittable> m_World;
        std::shared_ptr<Hittable> m_Light;
        Camera m_Camera;
        std::string m_Name = "Scene";
    };

} // namespace DSM

#endif // __SCENE_H__
