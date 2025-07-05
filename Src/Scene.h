#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include "Geometry/HittableList.h"
#include "Camera.h"
#include <mutex>

namespace DSM {

    class Scene {
    public:
        Scene(std::shared_ptr<Hittable> world, std::shared_ptr<Hittable> light, Camera camera) 
            : m_World(world), m_Light(light), m_Camera(std::move(camera)) {}
        Scene(std::vector<std::shared_ptr<Hittable>> objects, 
            std::vector<std::shared_ptr<Hittable>> light,
            Camera camera)
            :m_World(std::make_shared<HittableList>(std::move(objects))), 
            m_Light(std::make_shared<HittableList>(std::move(light))),
            m_Camera(std::move(camera)) {}
        
        void AddObject(std::shared_ptr<Hittable> object);
        void AddObjects(const std::vector<std::shared_ptr<Hittable>>& objects);
        void Clear();

        void Render();
        const Image& GetCameraImage() const { return m_Camera.GetImage(); }

        void SetCamera(Camera camera);

    private:
        std::mutex m_Mutex{};
        std::vector<std::shared_ptr<Hittable>> m_ObjCache;

        std::shared_ptr<Hittable> m_World;
        std::shared_ptr<Hittable> m_Light;
        Camera m_Camera;
    };

} // namespace DSM

#endif // __SCENE_H__