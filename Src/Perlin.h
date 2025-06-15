#pragma once
#ifndef __PERLIN_H__
#define __PERLIN_H__

#include <array>
#include <span>
#include <numeric>
#include "Pubh.h"
#include "Math/Vector.h"


namespace DSM {
    
    class Perlin{
    public:
        Perlin() noexcept
        {
            std::generate(m_RandomVec.begin(), m_RandomVec.end(), 
                [](){ return RandomUnitVector3f(); });

            PerlinGeneratePerm(m_PermX);
            PerlinGeneratePerm(m_PermY);
            PerlinGeneratePerm(m_PermZ);
        }

        float Noice(const Vector3f& p) const noexcept
        {
            int i = std::floor(p[0]);
            int j = std::floor(p[1]);
            int k = std::floor(p[2]);

            float u = p[0] - i;
            float v = p[1] - j;
            float w = p[2] - k;

            Vector3f c[2][2][2];
            for(int di = 0; di < 2; ++di){
                for(int dj = 0; dj < 2; ++dj){
                    for(int dk = 0; dk < 2; ++dk){
                        c[di][dj][dk] = m_RandomVec[
                            m_PermX[(i + di) & 255] ^
                            m_PermY[(j + dj) & 255] ^ 
                            m_PermZ[(k + dk) & 255]];
                    }
                }
            }

            return PerlinInterp(c, u, v, w);
        }

        float Turb(const Vector3f& p, int depth) const {
            auto accum = 0.f;
            auto temp_p = p;
            auto weight = 1.f;

            for (int i = 0; i < depth; i++) {
                accum += weight * Noice(temp_p);
                weight *= 0.5f;
                temp_p *= 2;
            }

            return std::fabs(accum);
        }


    private:
        static void PerlinGeneratePerm(std::span<int> perm)
        {
            std::iota(perm.begin(), perm.end(), 0);
            Permute(perm);
        }

        static void Permute(std::span<int> perm)
        {
            for(int i = perm.size() - 1; i > 0; --i){
                int target = RandomInt(0, i + 1);
                std::swap(perm[i], perm[target]); 
            }
        }

        static float PerlinInterp(Vector3f c[2][2][2], float u, float v, float w)
        {
            float uu = u*u*(3-2*u);
            float vv = v*v*(3-2*v);
            float ww = w*w*(3-2*w);
            float accum = 0.f;

            for (int i=0; i < 2; i++) {
                for (int j=0; j < 2; j++) {
                    for (int k=0; k < 2; k++) {
                        Vector3f weight_v{u-i, v-j, w-k};
                        accum += (i*uu + (1-i)*(1-uu))
                            * (j*vv + (1-j)*(1-vv))
                            * (k*ww + (1-k)*(1-ww))
                            * Vector3f::Dot(c[i][j][k], weight_v);
                    }
                }
            }
            
            return accum;
        }

    public:
        static constexpr std::size_t sm_PointCount = 256;

    private:
        std::array<Vector3f, sm_PointCount> m_RandomVec{};
        std::array<int, sm_PointCount> m_PermX{};
        std::array<int, sm_PointCount> m_PermY{};
        std::array<int, sm_PointCount> m_PermZ{};


    };

} // namespace DSM 

    


#endif