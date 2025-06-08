#pragma once
#ifndef __HITTABLELIST__H__
#define __HITTABLELIST__H__

#include "Hittable.h"
#include <memory>
#include <vector>
#include <shared_mutex>

namespace DSM {

    // 多线程中进行物体的添加需要进行互斥
    class HittableList : Hittable
    {
    public:
        void Add(std::shared_ptr<Hittable> hittable);
        void Clear() noexcept;

        bool Hit(const Ray& ray, HitRecord& hitRecord, Intervalf interval = Intervalf{}) const override;
        

    private:
        std::vector<std::shared_ptr<Hittable>> m_Objects{};
        mutable std::shared_mutex m_Mutex{};    // 共享互斥锁，可多个线程同时调用Hit
    };
}


#endif

