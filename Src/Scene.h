#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include "Geometry/HittableList.h"
#include "Camera.h"
#include <mutex>
#include <string>

namespace DSM {

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
        const Image& GetCameraImage() const { return m_Camera.GetImage(); }

        void SetCamera(Camera camera);

        // 场景名称（用于渲染耗时报告）
        const std::string& GetName() const noexcept { return m_Name; }

        // 渲染设置转发到内部相机
        void SetMaxDepth(int depth) noexcept { m_Camera.m_MaxDepth = depth; }
        void SetUseMultithread(bool useMultithread) noexcept { m_Camera.m_UseMultithread = useMultithread; }
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