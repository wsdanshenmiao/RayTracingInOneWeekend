#pragma once
#ifndef __PDF_H__
#define __PDF_H__

#include "ONB.h"
#include "Pubh.h"

namespace DSM {
    struct PDF
    {
        virtual ~PDF() = default;

        // 返回采样的方向
        virtual Vector3f Generate() const = 0;

        // 返回采样的概率密度函数值
        virtual float Value(const Vector3f& direction) const = 0;
    };

    class CosinePDF : public PDF
    {
    public:
        CosinePDF(const Vector3f& w) : m_ONB(w) {}

        Vector3f Generate() const override
        {
            return m_ONB.Transform(RandomCosineDirection());
        }

        float Value(const Vector3f& direction) const override
        {
            auto cosTheta = Vector3f::Dot(m_ONB.GetW(), direction.Normalized());
            return std::max(cosTheta / PI, 0.f);
        }

    private:
        ONB m_ONB;
    };

    class SpherePDF : public PDF
    {
    public:
        Vector3f Generate() const override
        {
            return RandomUnitVector3f();
        }

        float Value(const Vector3f& direction) const override
        {
            return 1.f / (4 * PI);
        }
    };

    class HittablePDF : public PDF
    {
    public:
        HittablePDF(const Hittable& hittable, const Vector3f& origin)
            : m_Hittable(hittable), m_Origin(origin) {}
        
        Vector3f Generate() const override
        {
            return m_Hittable.Random(m_Origin);
        }

        float Value(const Vector3f& dir) const override
        {
            return m_Hittable.PDFValue(m_Origin, dir);
        }

    private:
        const Hittable& m_Hittable;
        Vector3f m_Origin;
    };
    

    class MixturePDF : PDF
    {
    public:
        MixturePDF(std::shared_ptr<PDF> pdf1, std::shared_ptr<PDF> pdf2)
        {
            m_PDFs[0] = pdf1;
            m_PDFs[1] = pdf2;
        }

        Vector3f Generate() const override
        {
            return RandomFloat() < 0.5f ? m_PDFs[0]->Generate() : m_PDFs[1]->Generate();
        }

        float Value(const Vector3f& direction) const override
        {
            return 0.5f * m_PDFs[0]->Value(direction) + 0.5f * m_PDFs[1]->Value(direction);
        }

    private:
        std::array<std::shared_ptr<PDF>, 2> m_PDFs;
    };


} // namespace DSM 

#endif // __PDF_H__