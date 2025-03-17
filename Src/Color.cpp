#include "Color.h"
#include "Math/Interval.h"

namespace DSM {
    Color::Color(float r, float g, float b)
        : m_Color({r, g, b}){
    }

    Vector3i Color::GetColor() const
    {
        static const Intervalf interval(0.000f, 0.999f);
        int r = int(interval.Clamp(m_Color[0]) * 255.999);
        int g = int(interval.Clamp(m_Color[1]) * 255.999);
        int b = int(interval.Clamp(m_Color[2]) * 255.999);

        return Vector3i({r, g, b});
    }

    Vector3f Color::GetColorOrigin() const
    {
        return m_Color;
    }

    Color& Color::operator+=(const Color& other)
    {
        m_Color += other.m_Color;
        return *this;
    }

    Color& Color::operator-=(const Color& other)
    {
        m_Color -= other.m_Color;
        return *this;
    }

    Color& Color::operator*=(float other)
    {
        m_Color *= other;
        return *this;
    }

    Color& Color::operator/=(float other)
    {
        m_Color /= other;
        return *this;
    }

    Color& Color::operator*=(const Color& other)
    {
        for (std::size_t i = 0; i < 3; i++) {
            m_Color[i] *= other.m_Color[i];
        }
        return *this;
    }

    Color& Color::operator/=(const Color& other)
    {
        for (std::size_t i = 0; i < 3; i++) {
            m_Color[i] /= other.m_Color[i];
        }
        return *this;
    }

    int Color::R() const
    {
        return int(m_Color[0] * 255.999);
    }

    int Color::G() const
    {
        return int(m_Color[1] * 255.999);
    }

    int Color::B() const
    {
        return int(m_Color[2] * 255.999);
    }

    Color operator+(const Color& left, const Color& right)
    {
        Color ret(left);
        return ret += right;
    }

    Color operator-(const Color& left, const Color& right)
    {
        Color ret(left);
        return ret -= right;
    }

    Color operator*(const Color& left, float right)
    {
        Color ret(left);
        return ret *= right;
    }

    Color operator*(float left, const Color& right)
    {
        Color ret(right);
        return ret *= left;
    }

    Color operator/(float left, const Color& right)
    {
        Color ret(right);
        return ret /= left;
    }

    Color operator/(const Color& left, float right)
    {
        Color ret(left);
        return ret /= right;
    }

    Color operator*(const Color& left, const Color& right)
    {
        Color ret(left);
        return ret *= right;
    }

    Color operator/(const Color& left, const Color& right)
    {
        Color ret(left);
        return ret /= right;
    }
}
