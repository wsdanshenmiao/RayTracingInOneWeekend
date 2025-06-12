#pragma once
#ifndef __HITTABLELIST__H__
#define __HITTABLELIST__H__

#include "Hittable.h"
#include "AABB.h"
#include <memory>
#include <vector>
#include <shared_mutex>

namespace DSM {

    // 多线程中进行物体的添加需要进行互斥
    class HittableList : public Hittable
    {
    public:
        HittableList() = default;
        HittableList(std::shared_ptr<Hittable> obj) { Add(obj); }
        HittableList(const HittableList&) = delete;
        HittableList& operator=(const HittableList&) = delete;
        HittableList(HittableList&&) = default;
        HittableList& operator=(HittableList&&) = default;

        void Add(std::shared_ptr<Hittable> hittable);
        void Clear() noexcept;

        bool Hit(const Ray& ray, HitRecord& hitRecord, Intervalf interval = Intervalf{}) const override;
		AABB BoundingBox() const noexcept override { return m_BoundingBox; }

        const std::vector<std::shared_ptr<Hittable>>& GetObjects() const;

    private:
        AABB m_BoundingBox{};
        std::vector<std::shared_ptr<Hittable>> m_Objects{};
        mutable std::shared_mutex m_Mutex{};    // 共享互斥锁，可多个线程同时调用Hit
    };
}


#endif

