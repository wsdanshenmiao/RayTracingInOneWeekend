#include "HittableList.h"

namespace DSM{
    void HittableList::Add(std::shared_ptr<Hittable> hittable)
    {
        if (hittable == nullptr) return;
        std::lock_guard lock{ m_Mutex };
		m_BoundingBox = AABB::Uion(m_BoundingBox, hittable->BoundingBox());
        m_Objects.emplace_back(hittable);
    }

    void HittableList::Clear() noexcept
    {
        std::lock_guard lock{ m_Mutex };
		m_BoundingBox = AABB{};
        m_Objects.clear();
    }

    std::optional<HitRecord> HittableList::Hit(const Ray& ray, Intervalf interval) const
    {
        std::shared_lock lock{ m_Mutex };

        std::optional<HitRecord> hitRecord;
        float closestSoFar = interval.GetMax();

        for (const auto& object : m_Objects) {
            if (auto tmpHitRecord = object->Hit(ray, Intervalf{interval.GetMin(), closestSoFar});
                tmpHitRecord.has_value()) {
                hitRecord = std::move(tmpHitRecord);
                closestSoFar = hitRecord->m_Time; // 更新最远的距离
            }
        }

        return hitRecord;
    }
    
    // 需要返回副本避免悬空引用
    std::vector<std::shared_ptr<Hittable>> HittableList::GetObjects() const
    {
        std::shared_lock lock{ m_Mutex };
        return m_Objects;
    }
}
