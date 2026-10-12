#pragma once
#ifndef __HITTABLELIST__H__
#define __HITTABLELIST__H__

#include "Hittable.h"
#include "AABB.h"
#include <memory>
#include <vector>

namespace DSM {

    // 构建阶段由调用方串行修改；发布给渲染线程后，列表及可达资源保持只读。
    // Add/Clear 不得与 Hit/PDFValue/Random/BoundingBox/GetObjects 并发执行。
    class HittableList : public Hittable
    {
    public:
        HittableList() = default;
        HittableList(std::shared_ptr<Hittable> obj) { Add(obj); }
        HittableList(std::vector<std::shared_ptr<Hittable>> objs);
        HittableList(const HittableList&) = delete;
        HittableList& operator=(const HittableList&) = delete;

        void Add(std::shared_ptr<Hittable> hittable);
        void Clear() noexcept;

        std::optional<HitRecord> Hit(const Ray& ray, Intervalf interval = Intervalf{}) const override;
		AABB BoundingBox() const noexcept override { return m_BoundingBox; }

        float PDFValue(const Vector3f& origin, const Vector3f& dir) const override;
        Vector3f Random(const Vector3f& origin) const override;

        std::vector<std::shared_ptr<Hittable>> GetObjects() const;

    private:
        AABB m_BoundingBox{};
        std::vector<std::shared_ptr<Hittable>> m_Objects{};
    };
}


#endif

