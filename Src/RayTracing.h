#pragma once
#ifndef __RAYTRACING__H__
#define __RAYTRACING__H__

#include "Color.h"
#include "Ray.h"
#include "Geometry/HittableList.h"

namespace DSM {
    class RayTracing
    {
    public:
        RayTracing(float aspectRatio, int width);

        void Render() const;
        
    private:
        const float m_AspectRatio;
        int m_Width;
        std::unique_ptr<HittableList> m_World;
    };

    
}

#endif
