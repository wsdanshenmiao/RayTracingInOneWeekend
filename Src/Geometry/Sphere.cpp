#include "Sphere.h"

namespace DSM {
    Sphere::Sphere(Vector3f center, float radius) noexcept
        :m_Center(center), m_Radius(std::max(0.f, radius)){
    }

    bool Sphere::Hit(const Ray& ray, HitRecord& hitRecord, Intervalf interval) const
    {
        auto oc = m_Center - ray.GetOrigin();
        float a = ray.GetDirection().SqrMagnitude();
        float h = ray.GetDirection() * oc;
        float c = oc.SqrMagnitude() - m_Radius * m_Radius;

        float discriminant = h * h -  a * c;
        if (discriminant < 0) { // 没有根则不相交
            return false;
        }
        float sqrtD = sqrt(discriminant);
        float root = (h - sqrtD) / a;  // 计算方程的根
        auto outRange = [&interval](const auto& root){return !interval.Surrounds(root);};
        if (outRange(root)) { //不再范围内
            root = (h + sqrtD) / a;
            if (outRange(root)) {
                return false;
            }
        }

        hitRecord.m_Time = root;
        hitRecord.m_Pos = ray.At(root);
        auto n = (hitRecord.m_Pos - m_Center) / m_Radius;
        hitRecord.SetFaceNormal(ray, n);

        return true;
    }

}