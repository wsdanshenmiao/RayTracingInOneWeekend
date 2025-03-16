#pragma once
#ifndef __CAMERA__H__
#define __CAMERA__H__

#include "Color.h"
#include "Geometry/HittableList.h"
#include "Math/Vector.h"

namespace DSM {
    class Camera
    {
    public:
        explicit Camera(float aspectRatio = 1, int width = 400) noexcept;

        void Render(const HittableList& world)const;


    private:
        Color GetColor(const Ray& ray, const HittableList& world)const;

    private:
        float m_AspectRatio = 1;
        int m_Width = 400;
        int m_Height;
        Vector3f m_Pos;
        Vector3f m_StartPixelCenter;
        Vector3f m_PixelDeltaU;
        Vector3f m_PixelDeltaV;
    };


    
}


#endif