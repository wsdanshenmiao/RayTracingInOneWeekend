#include "Scene.h"

namespace DSM {
    void Scene::AddObject(std::shared_ptr<Hittable> object) {
        m_Objects.Add(object);
    }

    void Scene::AddObjects(const std::vector<std::shared_ptr<Hittable>>& objects) {
        for(const auto& object : objects) {
            m_Objects.Add(object);
        }
    }

    void Scene::SetCamera(const Camera& camera) {
        m_Camera = camera;
    }
} // namespace DSM