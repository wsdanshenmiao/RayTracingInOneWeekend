#pragma once
#ifndef __INTERVAL__H__
#define __INTERVAL__H_

#include <algorithm>
#include <limits>

namespace DSM{
    // 一段区间
    template<typename T> requires std::is_arithmetic_v<T>
    class Interval
    {
    public:
        constexpr Interval() noexcept;
        constexpr Interval(T min, T max) noexcept;

        constexpr T Size() const noexcept;
        constexpr T Clamp(T value) const noexcept;
        // 判断值是否在该范围内
        bool Surrounds(T value) const noexcept;
        bool Contains(T value) const noexcept;
        constexpr T GetMin() const noexcept;
        constexpr T GetMax() const noexcept;

    private:
        T m_Min;
        T m_Max;
    };

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Interval<T>::Interval() noexcept
        : m_Min(std::numeric_limits<T>::lowest()), m_Max(std::numeric_limits<T>::max()){
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Interval<T>::Interval(T min, T max) noexcept
        : m_Min(min), m_Max(max){
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr T Interval<T>::Size() const noexcept
    {
        return m_Max - m_Min;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr T Interval<T>::Clamp(T value) const noexcept
    {
        return std::clamp(value, m_Min, m_Max);
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr T Interval<T>::GetMin() const noexcept
    {
        return m_Min;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr T Interval<T>::GetMax() const noexcept
    {
        return m_Max;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    bool Interval<T>::Surrounds(T value) const noexcept
    {
        return m_Min < value && value < m_Max;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    bool Interval<T>::Contains(T value) const noexcept
    {
        return m_Min <= value && value <= m_Max;
    }

    using Intervalf = Interval<float>;
    using Intervald = Interval<double>;
    using intervali = Interval<int>;
    
}

#endif