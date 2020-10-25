#pragma once

#include <cassert>
#include <cmath>

template <typename T>
class Vector3
{
public:
    Vector3();

    Vector3(T x, T y, T z);

    T getX() const;

    T getY() const;

    T getZ() const;

    Vector3<T> setX(T x) const;

    Vector3<T> setY(T y) const;

    Vector3<T> setZ(T z) const;

    Vector3<T> operator+(const Vector3<T> &other) const;

    Vector3<T> operator*(const T value) const;

    Vector3<T> operator/(const T value) const;

    T length() const;

    Vector3<T> normalize() const;

private:
    T x;
    T y;
    T z;

    T ZERO = { 0 };

    T ONE = { 1 };
};
