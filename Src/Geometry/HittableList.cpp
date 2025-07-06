#include "HittableList.h"

namespace DSM{
    HittableList::HittableList(std::vector<std::shared_ptr<Hittable>> objs)
        :m_Objects(std::move(objs)){
        for(const auto& obj : m_Objects){
            m_BoundingBox = AABB::Uion(m_BoundingBox, obj->BoundingBox());
        }
    }

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

    float HittableList::PDFValue(const Vector3f &origin, const Vector3f &dir) const
    {
        float weight = 1.f / m_Objects.size();
        float sum = 0;
        for(const auto& obj : m_Objects){
            sum += obj->PDFValue(origin, dir) * weight;
        }
        return sum;
    }

    Vector3f HittableList::Random(const Vector3f &origin) const
    {
        int size = int(m_Objects.size());
        return m_Objects[RandomInt(0, size)]->Random(origin);
    }

    // 需要返回副本避免悬空引用
    std::vector<std::shared_ptr<Hittable>> HittableList::GetObjects() const
    {
        std::shared_lock lock{ m_Mutex };
        return m_Objects;
    }
}
