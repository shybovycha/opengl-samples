#include "Vector3.h"

template<typename T>
Vector3<T>::Vector3() : x(ZERO), y(ZERO), z(ZERO) {}

template<typename T>
Vector3<T>::Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

template<typename T>
T Vector3<T>::getX() const
{
    return x;
}

template<typename T>
T Vector3<T>::getY() const
{
    return y;
}

template<typename T>
T Vector3<T>::getZ() const
{
    return z;
}

template<typename T>
Vector3<T> Vector3<T>::setX(T _x) const
{
    return Vector3<T>(_x, y, z);
}

template<typename T>
Vector3<T> Vector3<T>::setY(T _y) const
{
    return Vector3<T>(x, _y, z);
}

template<typename T>
Vector3<T> Vector3<T>::setZ(T _z) const
{
    return Vector3<T>(x, y, _z);
}

template<typename T>
Vector3<T> Vector3<T>::operator+(const Vector3<T>& other) const
{
    return Vector3<T>(x + other.getX(), y + other.getY(), z + other.getZ());
}

template<typename T>
Vector3<T> Vector3<T>::operator*(const T value) const
{
    return Vector3<T>(x * value, y * value, z * value);
}

template<typename T>
Vector3<T> Vector3<T>::operator/(const T value) const
{
    assert(("the value a vector is divided by must not be zero", value != ZERO));

    return Vector3<T>(x / value, y / value, z / value);
}

template<typename T>
T Vector3<T>::length() const
{
    return std::sqrt((x * x) + (y * y) + (z * z));
}

template<typename T>
Vector3<T> Vector3<T>::normalize() const
{
    const auto l = length();

    assert(("the vector being normalized should have non-zero length", l != ZERO));

    return Vector3<T>(x / l, y / l, z / l);
}
