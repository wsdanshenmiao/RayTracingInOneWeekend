#include "Scene.h"

namespace DSM {

    void Scene::AddObject(std::shared_ptr<Hittable> object) {
        m_Objects->Add(object);
    }

    void Scene::AddObjects(const std::vector<std::shared_ptr<Hittable>>& objects) {
        for(const auto& object : objects) {
            m_Objects->Add(object);
        }
    }

    void Scene::Clear()
    {
        m_Objects->Clear();
    }

    void Scene::Render()
    {
        m_Camera.Render(*m_Objects);
    }

    void Scene::SetCamera(Camera camera) 
    {
        m_Camera = std::move(camera);
    }
} // namespace DSM