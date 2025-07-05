#include "Quad.h"
#include "HittableList.h"

namespace DSM {
    Quad::Quad(Vector3f q, Vector3f u, Vector3f v, std::shared_ptr<Material> mat) noexcept
        :m_Q(q), m_U(u), m_V(v), m_Mat(mat){
        AABB b0{q, q + u + v};
        AABB b1{q + u, q + v};
        m_BoundingBox = AABB::Uion(b0, b1);

        m_Normal = Vector3f::Cross(m_U, m_V);
        m_W = m_Normal / Vector3f::Dot(m_Normal, m_Normal);
        m_Area = m_Normal.Magnitude();
        Vector3f::Normalize(m_Normal);
        m_D = Vector3f::Dot(m_Normal, m_Q);
    }

    std::optional<HitRecord> Quad::Hit(const Ray& ray, Intervalf interval) const
    {
        std::optional<HitRecord> result{};

        float nd = Vector3f::Dot(m_Normal, ray.GetDirection());
        if(std::abs(nd) < 1e-4f) return result;   // 接近平行则不相交

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

    float Quad::PDFValue(const Vector3f &origin, const Vector3f &direction) const
    {
        std::optional<HitRecord> rec{};
        if(rec = Hit(Ray{origin, direction}, Intervalf{0.001f, std::numeric_limits<float>::max()});
            !rec.has_value()) return 0;

        float disSquared = rec->m_Time * rec->m_Time * direction.SqrMagnitude();
        float cosine = std::abs(Vector3f::Dot(direction, rec->m_Normal) / direction.Magnitude());

        return disSquared / (m_Area * cosine);
    }

    Vector3f Quad::Random(const Vector3f &origin) const
    {
        Vector3f q = m_Q + RandomFloat() * m_U + RandomFloat() * m_V;
        return q - origin;
    }

    namespace Geometry {    
        std::shared_ptr<HittableList> Geometry::Box(const Vector3f &a, const Vector3f &b, std::shared_ptr<Material> mat)
        {
            auto sides = std::make_shared<HittableList>();

            // Construct the two opposite vertices with the minimum and maximum coordinates.
            auto min = Vector3f{std::fmin(a[0],b[0]), std::fmin(a[1],b[1]), std::fmin(a[2],b[2])};
            auto max = Vector3f{std::fmax(a[0],b[0]), std::fmax(a[1],b[1]), std::fmax(a[2],b[2])};

            auto dx = Vector3f{max[0] - min[0], 0, 0};
            auto dy = Vector3f{0, max[1] - min[1], 0};
            auto dz = Vector3f{0, 0, max[2] - min[2]};

            sides->Add(std::make_shared<Quad>(Vector3f{min[0], min[1], max[2]},  dx,  dy, mat)); // front
            sides->Add(std::make_shared<Quad>(Vector3f{max[0], min[1], max[2]}, -dz,  dy, mat)); // right
            sides->Add(std::make_shared<Quad>(Vector3f{max[0], min[1], min[2]}, -dx,  dy, mat)); // back
            sides->Add(std::make_shared<Quad>(Vector3f{min[0], min[1], min[2]},  dz,  dy, mat)); // left
            sides->Add(std::make_shared<Quad>(Vector3f{min[0], max[1], max[2]},  dx, -dz, mat)); // top
            sides->Add(std::make_shared<Quad>(Vector3f{min[0], min[1], min[2]},  dx,  dz, mat)); // bottom

            return sides;
        }
    } // namespace Geometry

} // namespace DSM

