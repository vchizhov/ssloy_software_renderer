#pragma once

template<typename T, int N>
struct tvecn
{
    T e[N];

    tvecn() {}

    tvecn(T s)
    {
        for (int i=0; i<N; ++i)
            e[i] = s;
    }

    template<typename ...Pack>
    tvecn(Pack...pack) : e{static_cast<T>(pack)...}
    {
        static_assert(sizeof...(Pack) == N, "Number of arguments not equal to vector dimensions.");
    }

    T& operator[](int i) { return e[i]; };
    const T& operator[](int i) const { return e[i]; };

    tvecn operator-() const
    {
        tvecn<T,N> res;
        for (int i=0; i<N; ++i)
            res[i] = -e[i];
        return res;
    }

    tvecn& operator+=(const tvecn& rhs)
    {
        for (int i=0; i<N; ++i)
            e[i] += rhs[i];
        return *this;
    }

    tvecn& operator-=(const tvecn& rhs)
    {
        for (int i=0; i<N; ++i)
            e[i] -= rhs[i];
        return *this;
    }

    tvecn& operator*=(const tvecn& rhs)
    {
        for (int i=0; i<N; ++i)
            e[i] *= rhs[i];
        return *this;
    }

    tvecn& operator/=(const tvecn& rhs)
    {
        for (int i=0; i<N; ++i)
            e[i] /= rhs[i];
        return *this;
    }

    tvecn& operator*=(const T& rhs)
    {
        for (int i=0; i<N; ++i)
            e[i] *= rhs;
        return *this;
    }

    tvecn& operator/=(const T& rhs)
    {
        for (int i=0; i<N; ++i)
            e[i] /= rhs;
        return *this;
    }
    
};

/* aliases */
template<typename T>
using tvec3 = tvecn<T,3>;

template<typename T>
using tvec4 = tvecn<T,4>;

using vec3 = tvec3<float>;
using vec4 = tvec4<float>;


/* functions with vectors */
template<typename T, int N>
tvecn<T,N> operator+(const tvecn<T,N>& lhs, const tvecn<T,N>& rhs)
{
    tvecn<T,N> res;
    for (int i=0; i<N; ++i)
        res[i] = lhs[i] + rhs[i];
    return res;
}

template<typename T, int N>
tvecn<T,N> operator-(const tvecn<T,N>& lhs, const tvecn<T,N>& rhs)
{
    tvecn<T,N> res;
    for (int i=0; i<N; ++i)
        res[i] = lhs[i] - rhs[i];
    return res;
}

template<typename T, int N>
tvecn<T,N> operator*(const tvecn<T,N>& lhs, const tvecn<T,N>& rhs)
{
    tvecn<T,N> res;
    for (int i=0; i<N; ++i)
        res[i] = lhs[i] * rhs[i];
    return res;
}

template<typename T, int N>
tvecn<T,N> operator/(const tvecn<T,N>& lhs, const tvecn<T,N>& rhs)
{
    tvecn<T,N> res;
    for (int i=0; i<N; ++i)
        res[i] = lhs[i] / rhs[i];
    return res;
}

template<typename T, int N>
tvecn<T,N> operator*(const tvecn<T,N>& lhs, const T& rhs)
{
    tvecn<T,N> res;
    for (int i=0; i<N; ++i)
        res[i] = lhs[i] * rhs;
    return res;
}

template<typename T, int N>
tvecn<T,N> operator*(const T& lhs, const tvecn<T,N>& rhs)
{
    tvecn<T,N> res;
    for (int i=0; i<N; ++i)
        res[i] = lhs * rhs[i];
    return res;
}

template<typename T, int N>
tvecn<T,N> operator/(const tvecn<T,N>& lhs, const T& rhs)
{
    tvecn<T,N> res;
    for (int i=0; i<N; ++i)
        res[i] = lhs[i] / rhs;
    return res;
}

template<typename T, int N>
tvecn<T,N> operator/(const T& lhs, const tvecn<T,N>& rhs)
{
    tvecn<T,N> res;
    for (int i=0; i<N; ++i)
        res[i] = lhs / rhs[i];
    return res;
}

template<typename T, int N>
T dot (const tvecn<T,N>& lhs, const tvecn<T, N>& rhs)
{
    T acc = 0;
    for (int i=0; i<N; ++i)
        acc += lhs[i] * rhs[i];
    return acc;
}

template<typename T>
tvecn<T,3> cross (const tvecn<T,3>& lhs, const tvecn<T,3>& rhs)
{
    return tvecn<T,3>(lhs[1]*rhs[2]-lhs[2]*rhs[1], lhs[2]*rhs[0]-lhs[0]*rhs[2], lhs[0]*rhs[1]-lhs[1]*rhs[0]);
}

template<typename T, int N>
T length(const tvecn<T,N>& arg)
{
    return sqrt(dot(arg,arg));
}

template<typename T, int N>
tvecn<T,N> normalize(const tvecn<T,N>& arg)
{
    return arg / length(arg);
}