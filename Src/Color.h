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
        Vector3f GetColorOrigin() const noexcept;

        Color& operator+=(const Color& other) noexcept;
        Color& operator-=(const Color& other) noexcept;
        Color& operator*=(float other) noexcept;
        Color& operator/=(float other) noexcept;
        Color& operator*=(const Color& other);
        Color& operator/=(const Color& other);

        int R() const;
        int G() const;
        int B() const;

    private:
        float LinearToGamma(float linear) const noexcept;
        

    private:
        Vector3f m_Color;
    };

    Color operator+(const Color& left, const Color& right) noexcept;
    Color operator-(const Color& left, const Color& right) noexcept;
    Color operator*(const Color& left, float right) noexcept;
    Color operator*(float left, const Color& right) noexcept;
    Color operator/(float left, const Color& right) noexcept;
    Color operator/(const Color& left, float right) noexcept;
    Color operator*(const Color& left, const Color& right);
    Color operator/(const Color& left, const Color& right);
    
}


#endif