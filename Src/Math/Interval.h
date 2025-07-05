#pragma once
#ifndef __INTERVAL__H__
#define __INTERVAL__H_

#include <algorithm>
#include <limits>
#include <compare>

namespace DSM{
    // 一段区间
    template<typename T> requires std::is_arithmetic_v<T>
    class Interval
    {
    public:
        constexpr Interval() noexcept;
        constexpr Interval(T min, T max) noexcept;
        constexpr Interval(const Interval&) noexcept = default;
        constexpr Interval(Interval&&) noexcept = default;
        constexpr Interval& operator=(const Interval&) noexcept = default;
        constexpr Interval& operator=(Interval&&) noexcept = default;

        constexpr std::partial_ordering operator<=>(const Interval& other) const noexcept;
        constexpr bool operator==(const Interval& other) const noexcept = default;
        constexpr Interval& operator+=(T val) noexcept;
        constexpr Interval& operator-=(T val) noexcept;

        constexpr T Size() const noexcept;
        constexpr T Clamp(T value) const noexcept;
        constexpr Interval Expand(T delta)const noexcept;
        // 判断值是否在该范围内
        bool Surrounds(T value) const noexcept;
        bool Contains(T value) const noexcept;
        constexpr T GetMin() const noexcept;
        constexpr T GetMax() const noexcept;

        void SetMin(T val) noexcept;
        void SetMax(T val) noexcept;

        static constexpr Interval Intersection(const Interval& i0, const Interval& i1);    // 交集
        static constexpr Interval Uion(const Interval& i0, const Interval& i1);    // 并集

    public:
        static const Interval sm_Empty;
        static const Interval sm_Universe;

    private:
        T m_Min;
        T m_Max;
    };

    
    template <typename T> requires std::is_arithmetic_v<T>
    const Interval<T> Interval<T>::sm_Empty = Interval{};

    template <typename T> requires std::is_arithmetic_v<T>
    const Interval<T> Interval<T>::sm_Universe = 
            Interval{std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max()};

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Interval<T>::Interval() noexcept
        : m_Min(std::numeric_limits<T>::max()), m_Max(std::numeric_limits<T>::lowest()){
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Interval<T>::Interval(T min, T max) noexcept
        : m_Min(min), m_Max(max){
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr std::partial_ordering Interval<T>::operator<=>(const Interval &other) const noexcept
    {
        if(m_Min == other.m_Min && m_Max == other.m_Max){
            return std::partial_ordering::equivalent;
        }
        else if(m_Min <= other.m_Min && m_Max >= other.m_Max){
            return std::partial_ordering::greater;
        }
        else if(m_Min >= other.m_Min && m_Max <= other.m_Max){
            return std::partial_ordering::less;
        }
        return std::partial_ordering::unordered;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Interval<T> &Interval<T>::operator+=(T val) noexcept
    {
        m_Min += val;
        m_Max += val;
        return *this;
    }

    template <typename T> requires std::is_arithmetic_v<T>
    constexpr Interval<T> &Interval<T>::operator-=(T val) noexcept
    {
        m_Min -= val;
        m_Max -= val;
        return *this;
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

	template<typename T> requires std::is_arithmetic_v<T>
    constexpr Interval<T> Interval<T>::Expand(T delta) const noexcept
    {
        T halfDelta = delta * .5f;
        return Interval(m_Min - halfDelta, m_Max + halfDelta);
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

    template<typename T> requires std::is_arithmetic_v<T>
    void Interval<T>::SetMin(T val) noexcept
    {
        m_Min = val;
    }

    template<typename T> requires std::is_arithmetic_v<T>
    void Interval<T>::SetMax(T val) noexcept
    {
        m_Max = val;
    }

    template<typename T> requires std::is_arithmetic_v<T>
    constexpr Interval<T> Interval<T>::Intersection(const Interval& i0, const Interval& i1)
    {
        return Interval{ std::max(i0.m_Min, i1.m_Min), std::min(i0.m_Max, i1.m_Max) };
    }

    template<typename T> requires std::is_arithmetic_v<T>
    constexpr Interval<T> Interval<T>::Uion(const Interval& i0, const Interval& i1)
    {
        return Interval{ std::min(i0.m_Min, i1.m_Min), std::max(i0.m_Max, i1.m_Max)};
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

    
    
    
    
    
    template <typename T> requires std::is_arithmetic_v<T>
    Interval<T> operator+(Interval<T> i, T val) { return i += val; }
    template <typename T> requires std::is_arithmetic_v<T>
    Interval<T> operator+(T val, Interval<T> i) { return i += val; }    
    template <typename T> requires std::is_arithmetic_v<T>
    Interval<T> operator-(Interval<T> i, T val) { return i -= val; }
    template <typename T> requires std::is_arithmetic_v<T>
    Interval<T> operator-(T val, Interval<T> i) { return i -= val; }
    
    
    
    
    
    using Intervalf = Interval<float>;
    using Intervald = Interval<double>;
    using intervali = Interval<int>;
    
}

#endif