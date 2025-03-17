#pragma once
#ifndef __RAYTRACING__H__
#define __RAYTRACING__H__

#include "Geometry/HittableList.h"

namespace DSM {
    class HittableList;
    
    
    class RayTracing
    {
    public:
        RayTracing(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel);

        void Render() const;
        
    private:
        const float m_AspectRatio;
        std::uint32_t m_Width;
        std::uint32_t m_SamplePerPixel;
        std::unique_ptr<HittableList> m_World;
    };

    
}

#endif
