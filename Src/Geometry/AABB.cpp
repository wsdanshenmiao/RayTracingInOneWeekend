#include "AABB.h"
#include "../Math/Vector.h"
#include "../Ray.h"


namespace DSM {
    AABB::AABB(const Vector3f &min, const Vector3f &max)
    {
        m_X = (min[0] <= max[0]) ? Intervalf{min[0], max[0]} : Intervalf{max[0], min[0]};
        m_Y = (min[1] <= max[1]) ? Intervalf{min[1], max[1]} : Intervalf{max[1], min[1]};
        m_Z = (min[2] <= max[2]) ? Intervalf{min[2], max[2]} : Intervalf{max[2], min[2]};

        PadToMinimums();
    }

    Intervalf AABB::operator[](std::size_t index) const
    {
        switch (index)
        {
        case 0: return m_X;
        case 1: return m_Y;
        case 2: return m_Z;
        default: throw std::out_of_range{ "Index out of range" };
        }
        return Intervalf{ 0, 0 };
    }

    AABB &AABB::operator+=(const Vector3f &offset)
    {
        m_X += offset[0];
        m_Y += offset[1];
        m_Z += offset[2];
        return *this;
    }

    AABB &AABB::operator-=(const Vector3f &offset)
    {
        m_X -= offset[0];
        m_Y -= offset[1];
        m_Z -= offset[2];
        return *this;
    }

    bool AABB::Hit(const Ray& ray, Intervalf rayT) const
    {
        const Vector3f& origin = ray.GetOrigin();
        const Vector3f& dir = ray.GetDirection();

        for (std::size_t i = 0; i < 3; ++i) {
			const Intervalf& interval = (*this)[i];
			const float invDir = 1.0f / dir[i];

			float t0 = (interval.GetMin() - origin[i]) * invDir;
			float t1 = (interval.GetMax() - origin[i]) * invDir;

			const float& tmin = (t0 < t1) ? t0 : t1;
            const float& tmax = (t0 < t1) ? t1 : t0;

            if (tmin > rayT.GetMin()) rayT.SetMin(tmin);
            if (tmax < rayT.GetMax()) rayT.SetMax(tmax);

            if (rayT.GetMin() >= rayT.GetMax()) return false;
        }

        return true;
    }

    std::size_t AABB::LongestAxis() const noexcept
    {
        std::size_t ret = 0;
        if(m_X.Size() > m_Y.Size()){
            ret = m_X.Size() > m_Z.Size() ? 0 : 2;
        }
        else{
            ret = m_Y.Size() > m_Z.Size() ? 1 : 2;
        }
        return ret;
    }

    AABB AABB::Intersection(const AABB& box0, const AABB& box1)
    {
        AABB ret{};
        ret.m_X = Intervalf::Intersection(box0.m_X, box1.m_X);
        ret.m_Y = Intervalf::Intersection(box0.m_Y, box1.m_Y);
        ret.m_Z = Intervalf::Intersection(box0.m_Z, box1.m_Z);
        ret.PadToMinimums();
        return ret;
    }

    AABB AABB::Uion(const AABB& box0, const AABB& box1)
    {
        AABB ret{};
		ret.m_X = Intervalf::Uion(box0.m_X, box1.m_X);
		ret.m_Y = Intervalf::Uion(box0.m_Y, box1.m_Y);
		ret.m_Z = Intervalf::Uion(box0.m_Z, box1.m_Z);
        ret.PadToMinimums();
		return ret;
    }

    void AABB::PadToMinimums()
    {
        constexpr float detal = 0.0001f;

        if(m_X.Size() > detal) m_X = m_X.Expand(detal);
        if(m_Y.Size() > detal) m_Y = m_Y.Expand(detal);
        if(m_Z.Size() > detal) m_Z = m_Z.Expand(detal);
    }

} // namespace DSM


