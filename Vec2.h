#pragma once

template <typename T>
struct Vec2
{
    T x, y;

    Vec2() : x(0), y(0) {}

    Vec2(T x, T y) : x(x), y(y) {}

    Vec2(T s) : x(s), y(s) {}

    Vec2<T>& operator+=(const Vec2<T>& other) {
        *this = { x + other.x, y + other.y }
        return *this;
    }

    Vec2<T>& operator-() const {
        return { -x, -y };
    }

    Vec2<T>& operator-=(const Vec2<T>& other) {
        *this += -other;
        return *this;
    }

    Vec2<T>& operator*=(T scalar) {
        *this = { x * scalar, y * scalar }
        return *this;
    }

    Vec2<T>& operator*=(const Vec2<T>& other) {
        *this = { x * other.x, y * other.y }
        return *this;
    }

    Vec2<T>& operator/=(T scalar) {
        *this = { x / scalar, y / scalar }
        return *this;
    }

    template <typename U>
    Vec2<U> cast() const
    {
        return { static_cast<U>(x), static_cast<U>(y) };
    }
};

template <class T>
Vec2<T> operator+(const Vec2<T>& lhs, const Vec2<T>& rhs) {
    auto result = lhs;
    result += rhs;
    return result;
}

template <class T>
Vec2<T> operator-(const Vec2<T>& lhs, const Vec2<T>& rhs) {
    auto result = lhs;
    result -= rhs;
    return result;
}

template <class T>
Vec2<T> operator*(const Vec2<T>& lhs, T rhs) {
    auto result = lhs;
    result *= rhs;
    return result;
}

template <class T>
Vec2<T> operator*(T lhs, const Vec2<T>& rhs) {
    return rhs * lhs;
}

template <class T>
Vec2<T> operator*(const Vec2<T>& lhs, const Vec2<T>& rhs) {
    auto result = lhs;
    result *= rhs;
    return result;
}

template <class T>
Vec2<T> operator/(const Vec2<T>& lhs, T rhs) {
    auto result = lhs;
    result /= rhs;
    return result;
}

using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;
