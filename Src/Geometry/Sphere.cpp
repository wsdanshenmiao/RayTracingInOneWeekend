#include "Sphere.h"
#include "../Material.h"
#include "../Pubh.h"
#include "../Math/ONB.h"

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

    std::optional<HitRecord> Sphere::Hit(const Ray& ray, Intervalf interval) const
    {
        std::optional<HitRecord> ret;

        Vector3f center = m_Center.At(ray.GetTime());
        auto oc = center - ray.GetOrigin();
        float a = ray.GetDirection().SqrMagnitude();
        float h = ray.GetDirection() * oc;
        float c = oc.SqrMagnitude() - m_Radius * m_Radius;

        float discriminant = h * h -  a * c;
        if (discriminant < 0) { // 没有根则不相交
            return ret;
        }
        float sqrtD = std::sqrt(discriminant);
        float root = (h - sqrtD) / a;  // 计算方程的根
        if (!interval.Surrounds(root)) { //不再范围内
            root = (h + sqrtD) / a;
            if (!interval.Surrounds(root)) {
                return ret;
            }
        }

        HitRecord hitRecord;
        hitRecord.m_Pos = ray.At(root);
        auto n = (hitRecord.m_Pos - center) / m_Radius;
        hitRecord.SetFaceNormal(ray, n);
        hitRecord.m_UV = GetUV(hitRecord.m_Normal);
        hitRecord.m_Time = root;
        hitRecord.m_Material = m_Material;
        ret = std::make_optional(std::move(hitRecord));

        return ret;
    }

    // float Sphere::PDFValue(const Vector3f& origin, const Vector3f& dir) const
    // {
    //     auto rec = Hit(Ray{origin, dir}, Intervalf{0.001f, std::numeric_limits<float>::max()});
    //     if(!rec.has_value()) return 0;

    //     float sqrMagnitude = (origin - m_Center.GetOrigin()).SqrMagnitude();
    //     float sinTheta2 = m_Radius * m_Radius / sqrMagnitude;
    //     float cosThetaMax = std::sqrt(1 - sinTheta2);
    //     float solidAngle = 2 * PI * (1 - cosThetaMax);

    //     return 1.f / solidAngle;
    // }

    // Vector3f Sphere::Random(const Vector3f &origin) const
    // {
    //     Vector3f dir = m_Center.GetOrigin() - origin;
    //     ONB onb{dir};
    //     return onb.Transform(RandomInSphere(m_Radius, dir.SqrMagnitude()));
    // }

    Vector2f Sphere::GetUV(Vector3f pos) noexcept
    {
        float theta = std::acos(pos[1]);
        float phi = std::atan2(pos[2], pos[0]);

        float u = (phi + PI) / (2 * PI);
        float v = (theta) / PI;
        return Vector2f{ u, v };
    }
    
    Vector3f Sphere::RandomInSphere(float radius, float distanceSquared)
    {
        auto r1 = RandomFloat();
        auto r2 = RandomFloat();
        auto z = 1 + r2 * (std::sqrt(1 - radius * radius / distanceSquared) - 1);

        auto phi = 2 * PI * r1;
        auto x = std::cos(phi) * std::sqrt(1-z*z);
        auto y = std::sin(phi) * std::sqrt(1-z*z);

        return Vector3f{x, y, z};
    }




}