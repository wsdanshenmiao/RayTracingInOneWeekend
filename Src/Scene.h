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
        
        void AddObject(std::shared_ptr<Hittable> object);
        void AddObjects(const std::vector<std::shared_ptr<Hittable>>& objects);
        void SetCamera(const Camera& camera);

    private:
        HittableList m_Objects;
        Camera m_Camera;
    };

} // namespace DSM

#endif // __SCENE_H__