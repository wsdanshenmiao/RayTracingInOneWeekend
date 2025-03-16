#include "HittableList.h"

namespace DSM{
    void HittableList::Add(std::shared_ptr<Hittable> hittable)
    {
        m_Objects.emplace_back(hittable);
    }

    void HittableList::Clear() noexcept
    {
        m_Objects.clear();
    }

    bool HittableList::Hit(const Ray& ray, HitRecord& hitRecord, Intervalf interval) const
    {
        HitRecord tmpHitRecord;
        bool hitAnything = false;
        float closestSoFar = interval.GetMax();

        for (const auto& object : m_Objects) {
            if (object->Hit(ray, tmpHitRecord, Intervalf{interval.GetMin(), closestSoFar})) {
                hitAnything = true;
                closestSoFar = tmpHitRecord.m_Time; // 更新最远的距离
                hitRecord = tmpHitRecord;
            }
        }

        return hitAnything;
    }
}
