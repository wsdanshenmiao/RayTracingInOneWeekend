#pragma once
#ifndef __RAYTRACING__H__
#define __RAYTRACING__H__

#include "Geometry/HittableList.h"
#include "Camera.h"
#include "Scene.h"

namespace DSM {
    class HittableList;
	class Image;
    
    
    class RayTracing
    {
    public:
        RayTracing(float aspectRatio, std::uint32_t width, std::uint32_t samplePerPixel);

        void Render();
        const Scene& GetScene(std::size_t index) const; 

    private:
        void BuildScene0();
        void BuildScene1();
        
    private:
        const float m_AspectRatio;
        std::uint32_t m_Width;
        std::uint32_t m_SamplePerPixel;

        std::vector<Scene> m_Scenes;
    };

    
}

#endif
