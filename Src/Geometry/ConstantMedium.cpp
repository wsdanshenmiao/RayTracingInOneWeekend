#include "ConstantMedium.h"
#include "../Texture.h"

namespace DSM {
    ConstantMedium::ConstantMedium(std::shared_ptr<Hittable> boundary, float density, std::shared_ptr<Texture> tex)
        :m_Boundary(boundary), m_PhaseFunc(std::make_shared<IsotropicMat>(tex)), m_NegInvDensity(-1 / density) {
        if(density <= 0) throw std::invalid_argument{"Density should greater then zero"};
    }

    ConstantMedium::ConstantMedium(std::shared_ptr<Hittable> boundary, float density, const Color &col)
        :m_Boundary(boundary), m_PhaseFunc(std::make_shared<IsotropicMat>(col)), m_NegInvDensity(-1 / density) {
        if(density <= 0) throw std::invalid_argument{"Density should greater then zero"};
    }


    std::optional<HitRecord> ConstantMedium::Hit(const Ray& ray, Intervalf interval) const
    {
        std::optional<HitRecord> near, far;

        // 查找最近点
        if(near = m_Boundary->Hit(ray, Intervalf::sm_Universe); !near.has_value()) return {};
        // 查找最远点
        if(far = m_Boundary->Hit(ray, Intervalf{near->m_Time + 0.0001f, std::numeric_limits<float>::max()});
            !far.has_value()) return {};

        // 限制到 interval 内
        near->m_Time = std::max(near->m_Time, interval.GetMin());
        far->m_Time = std::min(far->m_Time, interval.GetMax());

        if(near->m_Time >= far->m_Time) return {};

        near->m_Time = std::max(near->m_Time, 0.f);

        float rayLen = ray.GetDirection().SqrMagnitude();
        float disInsideBoundary = (far->m_Time - near->m_Time) * rayLen;
        float randomDis = m_NegInvDensity * std::log(RandomFloat());

        // 随机退出
        if(randomDis > disInsideBoundary) return {};

        std::optional<HitRecord> result{};
        HitRecord rec{};
        rec.m_Time = near->m_Time + randomDis / rayLen;
        rec.m_Pos = ray.At(rec.m_Time);
        rec.m_UV = far->m_UV;
        rec.m_Normal = Vector3f{1,0,0};
        rec.m_FrontFace = true;
        rec.m_Material = m_PhaseFunc;
        result = std::make_optional(std::move(rec));

        return result;
    }

} // namespace DSM 
