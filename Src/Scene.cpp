#include "Scene.h"
#include <cassert>

namespace DSM {

    void Scene::AddObject(std::shared_ptr<Hittable> object) 
    {
        std::lock_guard lock{m_Mutex};
        m_ObjCache.push_back(object);
    }

    void Scene::AddObjects(const std::vector<std::shared_ptr<Hittable>>& objects) 
    {
        std::lock_guard lock{m_Mutex};
        m_ObjCache.append_range(objects);
    }

    void Scene::Clear()
    {
        std::lock_guard lock{m_Mutex};
        m_World = nullptr;
    }

    void Scene::Render()
    {
        std::lock_guard lock{m_Mutex};
        if(m_Light == nullptr) m_Light = std::make_shared<HittableList>();
        if(m_World == nullptr) m_World = std::make_shared<HittableList>();

        if(!m_ObjCache.empty()) 
        {
            m_ObjCache.push_back(m_World);
            m_World = std::make_shared<HittableList>(std::move(m_ObjCache));
            m_ObjCache.clear();
        }
        
        m_Camera.Render(*m_World, *m_Light);
    }

    void Scene::SetCamera(Camera camera) 
    {
        m_Camera = std::move(camera);
    }
} // namespace DSM