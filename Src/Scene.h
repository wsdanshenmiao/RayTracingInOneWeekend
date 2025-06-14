#pragma once
#ifndef __SCENE_H__
#define __SCENE_H__

#include "Geometry/Hittable.h"
#include "Geometry/HittableList.h"
#include "Camera.h"
#include <mutex>

namespace DSM {

    class Scene {
    public:
        Scene(const HittableList& objects, Camera camera) : Scene(objects.GetObjects(), std::move(camera)) {}
        Scene(std::vector<std::shared_ptr<Hittable>> objects, Camera camera)
            :m_Objects(std::make_unique<HittableList>(std::move(objects))), m_Camera(std::move(camera)) {}
        
        void AddObject(std::shared_ptr<Hittable> object);
        void AddObjects(const std::vector<std::shared_ptr<Hittable>>& objects);
        void Clear();

        void Render();
        const Image& GetCameraImage() const { return m_Camera.GetImage(); }

        void SetCamera(Camera camera);

    private:
        std::unique_ptr<HittableList> m_Objects;
        Camera m_Camera;
    };

} // namespace DSM

#endif // __SCENE_H__