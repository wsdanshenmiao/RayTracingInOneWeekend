#pragma once
#ifndef __HITTABLE__H__
#define __HITTABLE__H__

#include "../Ray.h"
#include "../Math/Interval.h"
#include "AABB.h"
#include "../Pubh.h"
#include <optional>

namespace DSM {
    struct Material;
    
    
    // 相交记录
    struct HitRecord
    {
        Vector3f m_Pos;
        Vector3f m_Normal;
        Vector2f m_UV;
        float m_Time;
        bool m_FrontFace;
        std::shared_ptr<Material> m_Material;

        void SetFaceNormal(const Ray& ray, const Vector3f& n)
        {
            m_FrontFace = (ray.GetDirection() * n) < 0;    // 光线是否在物体外部
            m_Normal = m_FrontFace ? n : -n;
        }
    };
    
    struct Hittable
    {
        virtual ~Hittable() = default;
        virtual std::optional<HitRecord> Hit(const Ray& ray, Intervalf interval = Intervalf{}) const = 0;
        virtual AABB BoundingBox() const noexcept = 0;
    };

    class Translate : public Hittable
    {
    public:
        Translate(std::shared_ptr<Hittable> obj, const Vector3f& offset)
            :m_Obj(obj), m_Offset(offset){
            m_BoundingBox = m_Obj->BoundingBox() + offset;
        }
        
        std::optional<HitRecord> Hit(const Ray& ray, Intervalf interval = Intervalf{}) const override
        {
            std::optional<HitRecord> result{};
            // 通过偏移光线来达到位移
            Ray offsetRay{ray.GetOrigin() - m_Offset, ray.GetDirection(), ray.GetTime()};
            if(result = m_Obj->Hit(offsetRay, interval); result.has_value()){
                result->m_Pos += m_Offset;
            }
            return result;
        }

        AABB BoundingBox() const noexcept override { return m_BoundingBox; }

    private:
        std::shared_ptr<Hittable> m_Obj{};
        AABB m_BoundingBox;
        Vector3f m_Offset{};
    };

    class RotateY : public Hittable
    {
    public:
        RotateY(std::shared_ptr<Hittable> obj, float angle)
            :m_Obj(obj){
            float radians = DegreesToRadians(angle);
            m_CosTheta = std::cos(radians);
            m_SinTheta = std::sin(radians);
            m_BoundingBox = m_Obj->BoundingBox();

            Vector3f min = Vector3f::PositiveInfinity();
            Vector3f max = Vector3f::NegativeInfinity();

            for(int i = 0; i < 2; ++i){
                for(int j = 0; j < 2; ++j){
                    for(int k = 0; k < 2; ++k){
                        float x = i * m_BoundingBox[0].GetMax() + (1 - i) * m_BoundingBox[0].GetMin();
                        float y = j * m_BoundingBox[1].GetMax() + (1 - j) * m_BoundingBox[1].GetMin();
                        float z = k * m_BoundingBox[2].GetMax() + (1 - k) * m_BoundingBox[2].GetMin();

                        float newX = m_CosTheta * x + m_SinTheta * z;
                        float newZ = -m_SinTheta * x + m_CosTheta * z;

                        Vector3f tester{newX, y, newZ};

                        for(int c = 0; c < 3; ++c){
                            min[c] = std::min(min[c], tester[c]);
                            max[c] = std::max(max[c], tester[c]);
                        }
                    }
                }
            }
            m_BoundingBox = AABB(min, max);
        }


        std::optional<HitRecord> Hit(const Ray& ray, Intervalf interval = Intervalf{}) const override
        {
            std::optional<HitRecord> result;

            auto rotateInv = [this](const Vector3f& v){
                return  Vector3f{v[0] * m_CosTheta - v[2] * m_SinTheta, v[1],
                    v[0] * m_SinTheta + v[2] * m_CosTheta};
            };            
            auto rotate = [this](const Vector3f& v){
                return  Vector3f{v[0] * m_CosTheta + v[2] * m_SinTheta, v[1],
                    (-m_SinTheta * v[0]) + (m_CosTheta * v[2])};
            };

            // 将光线从世界空间变换到局部空间，因此是乘上逆矩阵
            Vector3f origin = rotateInv(ray.GetOrigin());
            Vector3f dir = rotateInv(ray.GetDirection());

            Ray rotateRay{origin, dir, ray.GetTime()};
            if(result = m_Obj->Hit(rotateRay, interval); result.has_value()){
                result->m_Pos = rotate(result->m_Pos);
                result->m_Normal = rotate(result->m_Normal);
            }

            return result;
        }

        AABB BoundingBox() const noexcept override { return m_BoundingBox; }


    private:
        std::shared_ptr<Hittable> m_Obj;
        AABB m_BoundingBox{};
        float m_CosTheta;
        float m_SinTheta;
    };


}

#endif
