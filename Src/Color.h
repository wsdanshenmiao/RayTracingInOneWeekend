#pragma once
#ifndef __COLOR__H__
#define __COLOR__H__

#include "Math/Vector.h"

namespace DSM {
    class Color
    {
    public:
        Color() = default;
        Color(float r, float g, float b);
        
        Vector3i GetColor() const;
        Vector3f GetColorOrigin() const;

        Color& operator+=(const Color& other);
        Color& operator-=(const Color& other);
        Color& operator*=(float other);
        Color& operator/=(float other);
        Color& operator*=(const Color& other);
        Color& operator/=(const Color& other);

        int R() const;
        int G() const;
        int B() const;

    private:
        Vector3f m_Color;
    };

    Color operator+(const Color& left, const Color& right);
    Color operator-(const Color& left, const Color& right);
    Color operator*(const Color& left, float right);
    Color operator*(float left, const Color& right);
    Color operator/(float left, const Color& right);
    Color operator/(const Color& left, float right);
    Color operator*(const Color& left, const Color& right);
    Color operator/(const Color& left, const Color& right);
    
}


#endif