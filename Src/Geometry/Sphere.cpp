#include "Sphere.h"
#include "../Material.h"

namespace DSM {
    Sphere::Sphere(Vector3f center, float radius, std::shared_ptr<Material> mat) noexcept
        :m_Center(center, {}), m_Radius(std::max(0.f, radius)), m_Material(mat) {
        Vector3f rvec{ radius, radius, radius };
        m_BoundingBox = AABB{ m_Center.GetOrigin() - rvec, m_Center.GetOrigin() + rvec };
    }

    Sphere::Sphere(Vector3f center0, Vector3f center1, float radius, std::shared_ptr<Material> mat) noexcept
        :m_Center(center0, center1 - center0), m_Radius(std::max(0.f, radius)), m_Material(mat) {
        Vector3f rvec{ radius, radius, radius };
        AABB box0{ center0 - rvec, center0 + rvec };
        AABB box1{ center1 - rvec, center1 + rvec };
		m_BoundingBox = AABB::Uion(box0, box1);
    }

    bool Sphere::Hit(const Ray& ray, HitRecord& hitRecord, Intervalf interval) const
    {
        Vector3f center = m_Center.At(ray.GetTime());
        auto oc = center - ray.GetOrigin();
        float a = ray.GetDirection().SqrMagnitude();
        float h = ray.GetDirection() * oc;
        float c = oc.SqrMagnitude() - m_Radius * m_Radius;

        float discriminant = h * h -  a * c;
        if (discriminant < 0) { // 没有根则不相交
            return false;
        }
        float sqrtD = std::sqrt(discriminant);
        float root = (h - sqrtD) / a;  // 计算方程的根
        if (!interval.Surrounds(root)) { //不再范围内
            root = (h + sqrtD) / a;
            if (!interval.Surrounds(root)) {
                return false;
            }
        }

        hitRecord.m_Time = root;
        hitRecord.m_Pos = ray.At(root);
        hitRecord.m_Material = m_Material;
        auto n = (hitRecord.m_Pos - center) / m_Radius;
        hitRecord.SetFaceNormal(ray, n);

        return true;
    }

}