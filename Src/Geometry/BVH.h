#pragma once
#ifndef __BVH_H__
#define __BVH_H__

#include "Hittable.h"
#include "HittableList.h"
#include "AABB.h"

namespace DSM {
    
    class BVH : public Hittable{
    public:
        BVH(const HittableList& list) :BVH(list.GetObjects(), 0, list.GetObjects().size()) {}
        BVH(HittableList&& list) :BVH(list.GetObjects(), 0, list.GetObjects().size()) {}
        BVH(std::vector<std::shared_ptr<Hittable>> hittables, std::size_t begin, std::size_t end);

		bool Hit(const Ray& ray, HitRecord& rec, Intervalf interval) const override;
		AABB BoundingBox() const noexcept override { return m_BoundingBox; }

    private:
        static bool BoxCompare(
            const std::shared_ptr<Hittable> box0, 
            const std::shared_ptr<Hittable> box1, 
            std::size_t axisIndex);

    private:
        AABB m_BoundingBox{};
        std::shared_ptr<Hittable> m_Left{};
        std::shared_ptr<Hittable> m_Right{};
    };

} // namespace DS


#endif