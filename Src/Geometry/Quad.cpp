#include "Quad.h"

namespace DSM {
    Quad::Quad(Vector3f q, Vector3f u, Vector3f v, std::shared_ptr<Material> mat) noexcept
        :m_Q(q), m_U(u), m_V(v), m_Mat(mat){
        AABB b0{q, q + u + v};
        AABB b1{q + u, q + v};
        m_BoundingBox = AABB::Uion(b0, b1);

        m_Normal = Vector3f::Cross(m_U, m_V);
        m_W = m_Normal / Vector3f::Dot(m_Normal, m_Normal);
        Vector3f::Normalize(m_Normal);
        m_D = Vector3f::Dot(m_Normal, m_Q);
    }

    std::optional<HitRecord> Quad::Hit(const Ray& ray, Intervalf interval) const
    {
        std::optional<HitRecord> result{};

        float nd = Vector3f::Dot(m_Normal, ray.GetDirection());
        if(std::abs(nd) < 1e-6f) return result;   // 接近平行则不相交

        float t = (m_D - Vector3f::Dot(m_Normal, ray.GetOrigin())) / nd;
        if(!interval.Contains(t)) return result;

        Vector3f p = ray.At(t);

        Vector3f pq = p - m_Q;
        float alpha = Vector3f::Dot(m_W,  Vector3f::Cross(pq, m_V));
        float beta = Vector3f::Dot(m_W, Vector3f::Cross(m_U, pq));

        Intervalf range{0, 1};

        if(range.Contains(alpha) && range.Contains(beta)){
            HitRecord rec{};
            rec.m_Pos = p;
            rec.m_UV = Vector2f{alpha, beta};
            rec.m_Time = t;
            rec.m_Material = m_Mat;
            rec.SetFaceNormal(ray, m_Normal);
            result = std::make_optional(std::move(rec));
        }

        return result;
    }

} // namespace DSM 
