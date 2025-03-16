#pragma once
#ifndef __HITTABLELIST__H__
#define __HITTABLELIST__H__

#include "Hittable.h"
#include <memory>
#include <vector>


namespace DSM {
    struct HittableList : Hittable
    {
        void Add(std::shared_ptr<Hittable> hittable);
        void Clear() noexcept;

        bool Hit(const Ray& ray, HitRecord& hitRecord, Intervalf interval = Intervalf{}) const override;
        
        std::vector<std::shared_ptr<Hittable>> m_Objects;
    };
}


#endif

