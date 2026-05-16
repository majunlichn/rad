#pragma once

// 2D/3D geometry types and utilities built on glm.

#include <rad/Common/Float.h>
#include <rad/Common/Math.h>
#include <rad/Common/TypeTraits.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <cassert>
#include <limits>

namespace rad
{

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------

template <typename T>
    requires std::is_arithmetic_v<T>
using Vec2 = glm::vec<2, T, glm::defaultp>;

template <typename T>
    requires std::is_arithmetic_v<T>
using Vec3 = glm::vec<3, T, glm::defaultp>;

template <typename T>
    requires std::is_arithmetic_v<T>
using Vec4 = glm::vec<4, T, glm::defaultp>;

template <typename T>
    requires std::is_arithmetic_v<T>
struct Point2;

template <typename T>
    requires std::is_arithmetic_v<T>
struct Point3;

template <std::floating_point T>
struct Normal2;

template <std::floating_point T>
struct Normal3;

template <typename T>
    requires std::is_arithmetic_v<T>
struct Point2
{
    Vec2<T> m_pos{};

    constexpr Point2() noexcept = default;

    constexpr Point2(T x, T y) noexcept :
        m_pos(x, y)
    {
    }

    constexpr explicit Point2(T s) noexcept :
        m_pos(s)
    {
    }

    constexpr explicit Point2(Vec2<T> vec) noexcept :
        m_pos(vec)
    {
    }

    [[nodiscard]] constexpr const Vec2<T>& ToVec() const noexcept { return m_pos; }

    constexpr Point2& operator+=(Vec2<T> offset) noexcept
    {
        m_pos += offset;
        return *this;
    }

    constexpr Point2& operator+=(Point2 p) noexcept
    {
        m_pos += p.m_pos;
        return *this;
    }

    constexpr Point2& operator-=(Vec2<T> offset) noexcept
    {
        m_pos -= offset;
        return *this;
    }
}; // class Point2

template <typename T>
    requires std::is_arithmetic_v<T>
struct Point3
{
    Vec3<T> m_pos{};

    constexpr Point3() noexcept = default;

    constexpr Point3(T x, T y, T z) noexcept :
        m_pos(x, y, z)
    {
    }

    constexpr explicit Point3(T s) noexcept :
        m_pos(s)
    {
    }

    constexpr explicit Point3(Vec3<T> vec) noexcept :
        m_pos(vec)
    {
    }

    [[nodiscard]] constexpr const Vec3<T>& ToVec() const noexcept { return m_pos; }

    constexpr Point3& operator+=(Vec3<T> offset) noexcept
    {
        m_pos += offset;
        return *this;
    }

    constexpr Point3& operator+=(Point3 p) noexcept
    {
        m_pos += p.m_pos;
        return *this;
    }

    constexpr Point3& operator-=(Vec3<T> offset) noexcept
    {
        m_pos -= offset;
        return *this;
    }
}; // class Point3

template <std::floating_point T>
struct Normal2
{
    Vec2<T> m_dir{};

    constexpr Normal2() noexcept = default;

    constexpr Normal2(T x, T y) noexcept :
        m_dir(x, y)
    {
    }

    constexpr explicit Normal2(Vec2<T> vec) noexcept :
        m_dir(vec)
    {
    }

    [[nodiscard]] constexpr const Vec2<T>& ToVec() const noexcept { return m_dir; }

    constexpr Normal2& operator+=(Normal2 n) noexcept
    {
        m_dir += n.m_dir;
        return *this;
    }

    constexpr Normal2& operator*=(T s) noexcept
    {
        m_dir *= s;
        return *this;
    }
}; // class Normal2

template <std::floating_point T>
struct Normal3
{
    Vec3<T> m_dir{};

    constexpr Normal3() noexcept = default;

    constexpr Normal3(T x, T y, T z) noexcept :
        m_dir(x, y, z)
    {
    }

    constexpr explicit Normal3(Vec3<T> vec) noexcept :
        m_dir(vec)
    {
    }

    [[nodiscard]] constexpr const Vec3<T>& ToVec() const noexcept { return m_dir; }

    constexpr Normal3& operator+=(Normal3 n) noexcept
    {
        m_dir += n.m_dir;
        return *this;
    }

    constexpr Normal3& operator*=(T s) noexcept
    {
        m_dir *= s;
        return *this;
    }
}; // class Normal3

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Vec2<T> operator-(Point2<T> a, Point2<T> b) noexcept
{
    return a.ToVec() - b.ToVec();
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> operator+(Point2<T> a, Point2<T> b) noexcept
{
    return Point2<T>(a.ToVec() + b.ToVec());
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> operator+(Point2<T> p, Vec2<T> v) noexcept
{
    return Point2<T>(p.ToVec() + v);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> operator+(Vec2<T> v, Point2<T> p) noexcept
{
    return p + v;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> operator-(Point2<T> p, Vec2<T> v) noexcept
{
    return Point2<T>(p.ToVec() - v);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> operator-(Point2<T> p) noexcept
{
    return Point2<T>(-p.ToVec());
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> operator*(Point2<T> p, T s) noexcept
{
    return Point2<T>(p.ToVec() * s);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> operator*(T s, Point2<T> p) noexcept
{
    return p * s;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> operator/(Point2<T> p, T s) noexcept
{
    return Point2<T>(p.ToVec() / s);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Vec3<T> operator-(Point3<T> a, Point3<T> b) noexcept
{
    return a.ToVec() - b.ToVec();
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> operator+(Point3<T> a, Point3<T> b) noexcept
{
    return Point3<T>(a.ToVec() + b.ToVec());
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> operator+(Point3<T> p, Vec3<T> v) noexcept
{
    return Point3<T>(p.ToVec() + v);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> operator+(Vec3<T> v, Point3<T> p) noexcept
{
    return p + v;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> operator-(Point3<T> p, Vec3<T> v) noexcept
{
    return Point3<T>(p.ToVec() - v);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> operator-(Point3<T> p) noexcept
{
    return Point3<T>(-p.ToVec());
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> operator*(Point3<T> p, T s) noexcept
{
    return Point3<T>(p.ToVec() * s);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> operator*(T s, Point3<T> p) noexcept
{
    return p * s;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> operator/(Point3<T> p, T s) noexcept
{
    return Point3<T>(p.ToVec() / s);
}

template <std::floating_point T>
[[nodiscard]] constexpr Normal3<T> operator+(Normal3<T> a, Normal3<T> b) noexcept
{
    return Normal3<T>(a.ToVec() + b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr Normal3<T> operator-(Normal3<T> a, Normal3<T> b) noexcept
{
    return Normal3<T>(a.ToVec() - b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr Normal3<T> operator-(Normal3<T> n) noexcept
{
    return Normal3<T>(-n.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr Normal3<T> operator*(Normal3<T> n, T s) noexcept
{
    return Normal3<T>(n.ToVec() * s);
}

template <std::floating_point T>
[[nodiscard]] constexpr Normal3<T> operator*(T s, Normal3<T> n) noexcept
{
    return n * s;
}

template <std::floating_point T>
Normal3<T> operator+(Normal3<T>, Vec3<T>) = delete;

template <std::floating_point T>
Normal3<T> operator+(Vec3<T>, Normal3<T>) = delete;

template <std::floating_point T>
Normal3<T> operator-(Normal3<T>, Vec3<T>) = delete;

template <std::floating_point T>
Normal3<T> operator-(Vec3<T>, Normal3<T>) = delete;

template <std::floating_point T>
[[nodiscard]] constexpr Normal2<T> operator+(Normal2<T> a, Normal2<T> b) noexcept
{
    return Normal2<T>(a.ToVec() + b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr Normal2<T> operator-(Normal2<T> a, Normal2<T> b) noexcept
{
    return Normal2<T>(a.ToVec() - b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr Normal2<T> operator-(Normal2<T> n) noexcept
{
    return Normal2<T>(-n.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr Normal2<T> operator*(Normal2<T> n, T s) noexcept
{
    return Normal2<T>(n.ToVec() * s);
}

template <std::floating_point T>
[[nodiscard]] constexpr Normal2<T> operator*(T s, Normal2<T> n) noexcept
{
    return n * s;
}

template <std::floating_point T>
Normal2<T> operator+(Normal2<T>, Vec2<T>) = delete;

template <std::floating_point T>
Normal2<T> operator+(Vec2<T>, Normal2<T>) = delete;

template <std::floating_point T>
Normal2<T> operator-(Normal2<T>, Vec2<T>) = delete;

template <std::floating_point T>
Normal2<T> operator-(Vec2<T>, Normal2<T>) = delete;

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool operator==(Point2<T> a, Point2<T> b) noexcept
{
    return a.m_pos == b.m_pos;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool operator!=(Point2<T> a, Point2<T> b) noexcept
{
    return !(a == b);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool operator==(Point3<T> a, Point3<T> b) noexcept
{
    return a.m_pos == b.m_pos;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool operator!=(Point3<T> a, Point3<T> b) noexcept
{
    return !(a == b);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> Min(Point2<T> a, Point2<T> b) noexcept
{
    return Point2<T>(glm::min(a.ToVec(), b.ToVec()));
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> Max(Point2<T> a, Point2<T> b) noexcept
{
    return Point2<T>(glm::max(a.ToVec(), b.ToVec()));
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> Min(Point3<T> a, Point3<T> b) noexcept
{
    return Point3<T>(glm::min(a.ToVec(), b.ToVec()));
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> Max(Point3<T> a, Point3<T> b) noexcept
{
    return Point3<T>(glm::max(a.ToVec(), b.ToVec()));
}

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Point2f = Point2<float>;
using Point2d = Point2<double>;
using Point3f = Point3<float>;
using Point3d = Point3<double>;
using Normal2f = Normal2<float>;
using Normal2d = Normal2<double>;
using Normal3f = Normal3<float>;
using Normal3d = Normal3<double>;

template <typename T>
    requires std::is_arithmetic_v<T>
struct Bounds2;

template <typename T>
    requires std::is_arithmetic_v<T>
struct Bounds3;

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool Inside(Point2<T> p, const Bounds2<T>& bounds) noexcept;

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool InsideExclusive(Point2<T> p, const Bounds2<T>& bounds) noexcept;

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool Overlaps(const Bounds2<T>& a, const Bounds2<T>& b) noexcept;

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool Inside(Point3<T> p, const Bounds3<T>& bounds) noexcept;

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool InsideExclusive(Point3<T> p, const Bounds3<T>& bounds) noexcept;

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool Overlaps(const Bounds3<T>& a, const Bounds3<T>& b) noexcept;

// ---------------------------------------------------------------------------
// Scalar and vector utilities
// ---------------------------------------------------------------------------

template <std::floating_point T>
[[nodiscard]] constexpr bool AlmostEqual(Vec2<T> a, Vec2<T> b, T eps = Epsilon<T>) noexcept
{
    return AlmostEqual(a.x, b.x, eps) && AlmostEqual(a.y, b.y, eps);
}

template <std::floating_point T>
[[nodiscard]] constexpr bool AlmostEqual(Vec3<T> a, Vec3<T> b, T eps = Epsilon<T>) noexcept
{
    return AlmostEqual(a.x, b.x, eps) && AlmostEqual(a.y, b.y, eps) && AlmostEqual(a.z, b.z, eps);
}

template <std::floating_point T>
[[nodiscard]] constexpr bool AlmostEqual(Point2<T> a, Point2<T> b, T eps = Epsilon<T>) noexcept
{
    return AlmostEqual(a.m_pos, b.m_pos, eps);
}

template <std::floating_point T>
[[nodiscard]] constexpr bool AlmostEqual(Point3<T> a, Point3<T> b, T eps = Epsilon<T>) noexcept
{
    return AlmostEqual(a.m_pos, b.m_pos, eps);
}

template <std::floating_point T>
[[nodiscard]] constexpr bool AlmostEqual(Normal3<T> a, Normal3<T> b, T eps = Epsilon<T>) noexcept
{
    return AlmostEqual(a.m_dir, b.m_dir, eps);
}

template <std::floating_point T>
[[nodiscard]] constexpr bool AlmostEqual(Normal2<T> a, Normal2<T> b, T eps = Epsilon<T>) noexcept
{
    return AlmostEqual(a.m_dir, b.m_dir, eps);
}

template <std::floating_point T>
[[nodiscard]] constexpr Vec2<T> Abs(Vec2<T> v) noexcept
{
    return {std::abs(v.x), std::abs(v.y)};
}

template <std::floating_point T>
[[nodiscard]] constexpr Vec3<T> Abs(Vec3<T> v) noexcept
{
    return {std::abs(v.x), std::abs(v.y), std::abs(v.z)};
}

template <std::floating_point T>
[[nodiscard]] constexpr T Dot(Vec2<T> a, Vec2<T> b) noexcept
{
    return glm::dot(a, b);
}

template <std::floating_point T>
[[nodiscard]] constexpr T Dot(Vec3<T> a, Vec3<T> b) noexcept
{
    return glm::dot(a, b);
}

template <std::floating_point T>
[[nodiscard]] constexpr T Dot(Vec3<T> a, Normal3<T> b) noexcept
{
    return Dot(a, b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr T Dot(Normal3<T> a, Vec3<T> b) noexcept
{
    return Dot(a.ToVec(), b);
}

template <std::floating_point T>
[[nodiscard]] constexpr T Dot(Normal3<T> a, Normal3<T> b) noexcept
{
    return Dot(a.ToVec(), b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr T Dot(Vec2<T> a, Normal2<T> b) noexcept
{
    return Dot(a, b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr T Dot(Normal2<T> a, Vec2<T> b) noexcept
{
    return Dot(a.ToVec(), b);
}

template <std::floating_point T>
[[nodiscard]] constexpr T Dot(Normal2<T> a, Normal2<T> b) noexcept
{
    return Dot(a.ToVec(), b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr T AbsDot(Vec2<T> a, Vec2<T> b) noexcept
{
    return std::abs(Dot(a, b));
}

template <std::floating_point T>
[[nodiscard]] constexpr T AbsDot(Vec2<T> a, Normal2<T> b) noexcept
{
    return std::abs(Dot(a, b));
}

template <std::floating_point T>
[[nodiscard]] constexpr T AbsDot(Normal2<T> a, Vec2<T> b) noexcept
{
    return std::abs(Dot(a, b));
}

template <std::floating_point T>
[[nodiscard]] constexpr T AbsDot(Normal2<T> a, Normal2<T> b) noexcept
{
    return std::abs(Dot(a, b));
}

template <std::floating_point T>
[[nodiscard]] constexpr T AbsDot(Vec3<T> a, Vec3<T> b) noexcept
{
    return std::abs(Dot(a, b));
}

template <std::floating_point T>
[[nodiscard]] constexpr T AbsDot(Vec3<T> a, Normal3<T> b) noexcept
{
    return std::abs(Dot(a, b));
}

template <std::floating_point T>
[[nodiscard]] constexpr T AbsDot(Normal3<T> a, Vec3<T> b) noexcept
{
    return std::abs(Dot(a, b));
}

template <std::floating_point T>
[[nodiscard]] constexpr T AbsDot(Normal3<T> a, Normal3<T> b) noexcept
{
    return std::abs(Dot(a, b));
}

// 2D scalar cross product (signed parallelogram area).
template <std::floating_point T>
[[nodiscard]] constexpr T Cross(Vec2<T> a, Vec2<T> b) noexcept
{
    return a.x * b.y - a.y * b.x;
}

template <std::floating_point T>
[[nodiscard]] constexpr T Cross(Vec2<T> a, Normal2<T> b) noexcept
{
    return Cross(a, b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr T Cross(Normal2<T> a, Vec2<T> b) noexcept
{
    return Cross(a.ToVec(), b);
}

template <std::floating_point T>
[[nodiscard]] constexpr Vec3<T> Cross(Vec3<T> a, Vec3<T> b) noexcept
{
    return glm::cross(a, b);
}

template <std::floating_point T>
[[nodiscard]] constexpr Vec3<T> Cross(Vec3<T> a, Normal3<T> b) noexcept
{
    return Cross(a, b.ToVec());
}

template <std::floating_point T>
[[nodiscard]] constexpr Vec3<T> Cross(Normal3<T> a, Vec3<T> b) noexcept
{
    return Cross(a.ToVec(), b);
}

template <std::floating_point T>
[[nodiscard]] constexpr T LengthSquared(Vec2<T> v) noexcept
{
    return Dot(v, v);
}

template <std::floating_point T>
[[nodiscard]] constexpr T LengthSquared(Vec3<T> v) noexcept
{
    return Dot(v, v);
}

template <std::floating_point T>
[[nodiscard]] inline T Length(Vec2<T> v) noexcept
{
    return glm::length(v);
}

template <std::floating_point T>
[[nodiscard]] inline T Length(Vec3<T> v) noexcept
{
    return glm::length(v);
}

template <std::floating_point T>
[[nodiscard]] constexpr T DistanceSquared(Point2<T> a, Point2<T> b) noexcept
{
    return LengthSquared(b - a);
}

template <std::floating_point T>
[[nodiscard]] constexpr T DistanceSquared(Point3<T> a, Point3<T> b) noexcept
{
    return LengthSquared(b - a);
}

template <std::floating_point T>
[[nodiscard]] inline T Distance(Point2<T> a, Point2<T> b) noexcept
{
    return Length(b - a);
}

template <std::floating_point T>
[[nodiscard]] inline T Distance(Point3<T> a, Point3<T> b) noexcept
{
    return Length(b - a);
}

// Returns zero for a zero-length input.
template <std::floating_point T>
[[nodiscard]] inline Vec2<T> Normalize(Vec2<T> v) noexcept
{
    if (AlmostZero(LengthSquared(v))) [[unlikely]]
    {
        return Vec2<T>(0);
    }
    return glm::normalize(v);
}

template <std::floating_point T>
[[nodiscard]] inline Vec3<T> Normalize(Vec3<T> v) noexcept
{
    if (AlmostZero(LengthSquared(v))) [[unlikely]]
    {
        return Vec3<T>(0);
    }
    return glm::normalize(v);
}

template <std::floating_point T>
[[nodiscard]] inline Normal3<T> Normalize(Normal3<T> n) noexcept
{
    return Normal3<T>(Normalize(n.ToVec()));
}

template <std::floating_point T>
[[nodiscard]] inline Normal2<T> Normalize(Normal2<T> n) noexcept
{
    return Normal2<T>(Normalize(n.ToVec()));
}

template <std::floating_point T>
[[nodiscard]] constexpr Vec2<T> Lerp(T t, Vec2<T> a, Vec2<T> b) noexcept
{
    return glm::mix(a, b, t);
}

template <std::floating_point T>
[[nodiscard]] constexpr Vec3<T> Lerp(T t, Vec3<T> a, Vec3<T> b) noexcept
{
    return glm::mix(a, b, t);
}

template <std::floating_point T>
[[nodiscard]] constexpr Point2<T> Lerp(T t, Point2<T> a, Point2<T> b) noexcept
{
    return Point2<T>(Lerp(t, a.ToVec(), b.ToVec()));
}

template <std::floating_point T>
[[nodiscard]] constexpr Point3<T> Lerp(T t, Point3<T> a, Point3<T> b) noexcept
{
    return Point3<T>(Lerp(t, a.ToVec(), b.ToVec()));
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Vec2<T> Min(Vec2<T> a, Vec2<T> b) noexcept
{
    return glm::min(a, b);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Vec2<T> Max(Vec2<T> a, Vec2<T> b) noexcept
{
    return glm::max(a, b);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Vec3<T> Min(Vec3<T> a, Vec3<T> b) noexcept
{
    return glm::min(a, b);
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Vec3<T> Max(Vec3<T> a, Vec3<T> b) noexcept
{
    return glm::max(a, b);
}

template <std::floating_point T>
[[nodiscard]] constexpr T MinComponentValue(Vec2<T> v) noexcept
{
    return (std::min)(v.x, v.y);
}

template <std::floating_point T>
[[nodiscard]] constexpr T MaxComponentValue(Vec2<T> v) noexcept
{
    return (std::max)(v.x, v.y);
}

template <std::floating_point T>
[[nodiscard]] constexpr int MinComponentIndex(Vec2<T> v) noexcept
{
    return (v.x < v.y) ? 0 : 1;
}

template <std::floating_point T>
[[nodiscard]] constexpr int MaxComponentIndex(Vec2<T> v) noexcept
{
    return (v.x > v.y) ? 0 : 1;
}

template <std::floating_point T>
[[nodiscard]] constexpr T MinComponentValue(Vec3<T> v) noexcept
{
    return (std::min)(v.x, (std::min)(v.y, v.z));
}

template <std::floating_point T>
[[nodiscard]] constexpr T MaxComponentValue(Vec3<T> v) noexcept
{
    return (std::max)(v.x, (std::max)(v.y, v.z));
}

template <std::floating_point T>
[[nodiscard]] constexpr int MinComponentIndex(Vec3<T> v) noexcept
{
    return (v.x < v.y) ? ((v.x < v.z) ? 0 : 2) : ((v.y < v.z) ? 1 : 2);
}

template <std::floating_point T>
[[nodiscard]] constexpr int MaxComponentIndex(Vec3<T> v) noexcept
{
    return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}

// Component-wise product (x * y [* z]).
template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T Prod(Vec2<T> v) noexcept
{
    return v.x * v.y;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T Prod(Vec3<T> v) noexcept
{
    return v.x * v.y * v.z;
}

// 90-degree rotation in the XY plane.
template <std::floating_point T>
[[nodiscard]] constexpr Vec2<T> Rotate90CCW(Vec2<T> v) noexcept
{
    return Vec2<T>(-v.y, v.x);
}

template <std::floating_point T>
[[nodiscard]] constexpr Vec2<T> Rotate90CW(Vec2<T> v) noexcept
{
    return Vec2<T>(v.y, -v.x);
}

template <std::floating_point T>
struct OrthoFrame2;

template <std::floating_point T>
[[nodiscard]] OrthoFrame2<T> MakeOrthoFrame(Vec2<T> x) noexcept;

// Right-handed orthonormal basis in the plane: m_x, m_y expressed in world space.
template <std::floating_point T>
struct OrthoFrame2
{
    Vec2<T> m_x{1, 0};
    Vec2<T> m_y{0, 1};

    constexpr OrthoFrame2() noexcept = default;

    constexpr OrthoFrame2(Vec2<T> x, Vec2<T> y) noexcept :
        m_x(x),
        m_y(y)
    {
    }

    [[nodiscard]] static inline OrthoFrame2 FromX(Vec2<T> x) noexcept { return MakeOrthoFrame(x); }

    [[nodiscard]] constexpr Vec2<T> ToLocal(Vec2<T> v) const noexcept
    {
        return Vec2<T>(Dot(v, m_x), Dot(v, m_y));
    }

    [[nodiscard]] constexpr Vec2<T> FromLocal(Vec2<T> v) const noexcept
    {
        return m_x * v.x + m_y * v.y;
    }
}; // struct OrthoFrame2

// Builds a right-handed orthonormal frame; non-unit x is normalized (zero -> identity).
template <std::floating_point T>
[[nodiscard]] inline OrthoFrame2<T> MakeOrthoFrame(Vec2<T> x) noexcept
{
    if (AlmostZero(LengthSquared(x))) [[unlikely]]
    {
        return OrthoFrame2<T>();
    }
    const Vec2<T> xHat = Normalize(x);
    return OrthoFrame2<T>(xHat, Rotate90CCW(xHat));
}

// Reflects v about n. n should be unit length (glm convention).
template <std::floating_point T>
[[nodiscard]] inline Vec2<T> Reflect(Vec2<T> v, Normal2<T> n) noexcept
{
    return glm::reflect(v, n.ToVec());
}

template <std::floating_point T>
[[nodiscard]] inline Vec3<T> Reflect(Vec3<T> v, Normal3<T> n) noexcept
{
    return glm::reflect(v, n.ToVec());
}

// Stable angle between vectors; see https://www.plunk.org/~hatch/rightway.html
template <std::floating_point T>
[[nodiscard]] inline T AngleBetween(Vec2<T> a, Vec2<T> b) noexcept
{
    if (AlmostZero(LengthSquared(a)) || AlmostZero(LengthSquared(b))) [[unlikely]]
    {
        return T(0);
    }
    if (Dot(a, b) < T(0)) [[unlikely]]
    {
        return T(M_PI) - T(2) * std::asin((std::min)(T(1), Length(a + b) / T(2)));
    }
    return T(2) * std::asin((std::min)(T(1), Length(b - a) / T(2)));
}

template <std::floating_point T>
[[nodiscard]] inline T AngleBetween(Vec3<T> a, Vec3<T> b) noexcept
{
    if (AlmostZero(LengthSquared(a)) || AlmostZero(LengthSquared(b))) [[unlikely]]
    {
        return T(0);
    }
    if (Dot(a, b) < T(0)) [[unlikely]]
    {
        return T(M_PI) - T(2) * std::asin((std::min)(T(1), Length(a + b) / T(2)));
    }
    return T(2) * std::asin((std::min)(T(1), Length(b - a) / T(2)));
}

template <std::floating_point T>
struct OrthoFrame3;

template <std::floating_point T>
[[nodiscard]] OrthoFrame3<T> MakeOrthoFrame(Vec3<T> z) noexcept;

template <std::floating_point T>
[[nodiscard]] OrthoFrame3<T> MakeOrthoFrame(Normal3<T> z) noexcept;

// Right-handed orthonormal basis: axes m_x, m_y, m_z expressed in world space.
// Not a change of origin — only rotation (local coordinates via ToLocal / FromLocal).
template <std::floating_point T>
struct OrthoFrame3
{
    Vec3<T> m_x{1, 0, 0};
    Vec3<T> m_y{0, 1, 0};
    Vec3<T> m_z{0, 0, 1};

    constexpr OrthoFrame3() noexcept = default;

    constexpr OrthoFrame3(Vec3<T> x, Vec3<T> y, Vec3<T> z) noexcept :
        m_x(x),
        m_y(y),
        m_z(z)
    {
    }

    [[nodiscard]] static inline OrthoFrame3 FromZ(Vec3<T> z) noexcept { return MakeOrthoFrame(z); }

    [[nodiscard]] static inline OrthoFrame3 FromZ(Normal3<T> z) noexcept
    {
        return MakeOrthoFrame(z.ToVec());
    }

    [[nodiscard]] constexpr Vec3<T> ToLocal(Vec3<T> v) const noexcept
    {
        return Vec3<T>(Dot(v, m_x), Dot(v, m_y), Dot(v, m_z));
    }

    [[nodiscard]] constexpr Vec3<T> FromLocal(Vec3<T> v) const noexcept
    {
        return m_x * v.x + m_y * v.y + m_z * v.z;
    }
}; // struct OrthoFrame3

// Frisvad/PBRT orthonormal completion; non-unit z is normalized (zero -> identity).
template <std::floating_point T>
[[nodiscard]] inline OrthoFrame3<T> MakeOrthoFrame(Vec3<T> z) noexcept
{
    if (AlmostZero(LengthSquared(z))) [[unlikely]]
    {
        return OrthoFrame3<T>();
    }
    const Vec3<T> zHat = Normalize(z);
    const T sign = std::copysign(T(1), zHat.z);
    const T a = T(-1) / (sign + zHat.z);
    const T b = zHat.x * zHat.y * a;
    const Vec3<T> x(T(1) + sign * Square(zHat.x) * a, sign * b, -sign * zHat.x);
    const Vec3<T> y(b, sign + Square(zHat.y) * a, -zHat.y);
    return OrthoFrame3<T>(x, y, zHat);
}

template <std::floating_point T>
[[nodiscard]] inline OrthoFrame3<T> MakeOrthoFrame(Normal3<T> z) noexcept
{
    return MakeOrthoFrame(z.ToVec());
}

// Flips v to face the same hemisphere as reference (Dot >= 0).
template <std::floating_point T>
[[nodiscard]] inline Vec2<T> FaceForward(Vec2<T> v, Vec2<T> reference) noexcept
{
    return (Dot(v, reference) < T(0)) ? -v : v;
}

template <std::floating_point T>
[[nodiscard]] inline Vec2<T> FaceForward(Vec2<T> v, Normal2<T> reference) noexcept
{
    return FaceForward(v, reference.ToVec());
}

template <std::floating_point T>
[[nodiscard]] inline Normal2<T> FaceForward(Normal2<T> n, Vec2<T> reference) noexcept
{
    return (Dot(n, reference) < T(0)) ? -n : n;
}

template <std::floating_point T>
[[nodiscard]] inline Normal2<T> FaceForward(Normal2<T> n, Normal2<T> reference) noexcept
{
    return (Dot(n, reference) < T(0)) ? -n : n;
}

template <std::floating_point T>
[[nodiscard]] inline Vec3<T> FaceForward(Vec3<T> v, Vec3<T> reference) noexcept
{
    return (Dot(v, reference) < T(0)) ? -v : v;
}

template <std::floating_point T>
[[nodiscard]] inline Vec3<T> FaceForward(Vec3<T> v, Normal3<T> reference) noexcept
{
    return FaceForward(v, reference.ToVec());
}

template <std::floating_point T>
[[nodiscard]] inline Normal3<T> FaceForward(Normal3<T> n, Vec3<T> reference) noexcept
{
    return (Dot(n, reference) < T(0)) ? -n : n;
}

template <std::floating_point T>
[[nodiscard]] inline Normal3<T> FaceForward(Normal3<T> n, Normal3<T> reference) noexcept
{
    return (Dot(n, reference) < T(0)) ? -n : n;
}

// ---------------------------------------------------------------------------
// Axis-aligned bounds
// ---------------------------------------------------------------------------

// 2D axis-aligned bounding box. Supports floating-point and integral T (e.g. pixel rects).
template <typename T>
    requires std::is_arithmetic_v<T>
struct Bounds2
{
    Point2<T> m_min{};
    Point2<T> m_max{};

    constexpr Bounds2() noexcept :
        m_min(std::numeric_limits<T>::max()),
        m_max(std::numeric_limits<T>::lowest())
    {
    }

    constexpr explicit Bounds2(Point2<T> p) noexcept :
        m_min(p),
        m_max(p)
    {
    }

    constexpr Bounds2(Point2<T> p1, Point2<T> p2) noexcept :
        m_min(Min(p1, p2)),
        m_max(Max(p1, p2))
    {
    }

    [[nodiscard]] static constexpr Bounds2 FromPoints(Point2<T> a, Point2<T> b) noexcept
    {
        return Bounds2(a, b);
    }

    [[nodiscard]] static constexpr Bounds2 FromCenterHalfSize(Point2<T> center,
                                                              Vec2<T> halfSize) noexcept
    {
        return Bounds2(center - halfSize, center + halfSize);
    }

    [[nodiscard]] constexpr Point2<T> operator[](int i) const noexcept
    {
        assert(i == 0 || i == 1);
        return (i == 0) ? m_min : m_max;
    }

    [[nodiscard]] constexpr Point2<T>& operator[](int i) noexcept
    {
        assert(i == 0 || i == 1);
        return (i == 0) ? m_min : m_max;
    }

    [[nodiscard]] constexpr Vec2<T> Diagonal() const noexcept { return m_max - m_min; }

    [[nodiscard]] constexpr Point2<T> Center() const noexcept
    {
        if constexpr (std::floating_point<T>)
        {
            return (m_min + m_max) * T(0.5);
        }
        else
        {
            return (m_min + m_max) / T(2);
        }
    }

    [[nodiscard]] constexpr Vec2<T> Size() const noexcept { return Diagonal(); }

    [[nodiscard]] constexpr T Width() const noexcept { return m_max.m_pos.x - m_min.m_pos.x; }

    [[nodiscard]] constexpr T Height() const noexcept { return m_max.m_pos.y - m_min.m_pos.y; }

    [[nodiscard]] constexpr T Area() const noexcept
    {
        if (IsEmpty()) [[unlikely]]
        {
            return T(0);
        }
        return Width() * Height();
    }

    [[nodiscard]] constexpr T Perimeter() const noexcept
    {
        if (IsEmpty()) [[unlikely]]
        {
            return T(0);
        }
        return T(2) * (Width() + Height());
    }

    [[nodiscard]] constexpr bool IsEmpty() const noexcept
    {
        return m_min.m_pos.x >= m_max.m_pos.x || m_min.m_pos.y >= m_max.m_pos.y;
    }

    [[nodiscard]] constexpr bool IsDegenerate() const noexcept
    {
        return m_min.m_pos.x > m_max.m_pos.x || m_min.m_pos.y > m_max.m_pos.y;
    }

    [[nodiscard]] constexpr int MaxDimensionIndex() const noexcept
    {
        const Vec2<T> d = Diagonal();
        return (d.x > d.y) ? 0 : 1;
    }

    [[nodiscard]] constexpr Point2<T> GetCorner(int corner) const noexcept
    {
        assert(corner >= 0 && corner < 4);
        return Point2<T>((*this)[corner & 1].m_pos.x, (*this)[(corner & 2) ? 1 : 0].m_pos.y);
    }

    [[nodiscard]] constexpr Point2<T> Lerp(Vec2<T> t) const noexcept
    {
        return Point2<T>(glm::mix(m_min.m_pos.x, m_max.m_pos.x, t.x),
                         glm::mix(m_min.m_pos.y, m_max.m_pos.y, t.y));
    }

    [[nodiscard]] constexpr Vec2<T> NormalizedOffset(Point2<T> p) const noexcept
    {
        Vec2<T> o = p - m_min;
        if (m_max.m_pos.x > m_min.m_pos.x) [[likely]]
        {
            o.x /= m_max.m_pos.x - m_min.m_pos.x;
        }
        if (m_max.m_pos.y > m_min.m_pos.y) [[likely]]
        {
            o.y /= m_max.m_pos.y - m_min.m_pos.y;
        }
        return o;
    }

    [[nodiscard]] constexpr bool Contains(Point2<T> p) const noexcept { return Inside(p, *this); }

    [[nodiscard]] constexpr bool Intersects(Bounds2 other) const noexcept
    {
        return Overlaps(*this, other);
    }

    [[nodiscard]] constexpr Bounds2 Union(Bounds2 other) const noexcept
    {
        return Bounds2(Min(m_min, other.m_min), Max(m_max, other.m_max));
    }

    constexpr Bounds2& Expand(Point2<T> p) noexcept
    {
        m_min = Min(m_min, p);
        m_max = Max(m_max, p);
        return *this;
    }

    constexpr Bounds2& Expand(Bounds2 other) noexcept
    {
        m_min = Min(m_min, other.m_min);
        m_max = Max(m_max, other.m_max);
        return *this;
    }
};

template <typename T>
    requires std::is_arithmetic_v<T>
struct Bounds3
{
    Point3<T> m_min{};
    Point3<T> m_max{};

    constexpr Bounds3() noexcept :
        m_min(std::numeric_limits<T>::max()),
        m_max(std::numeric_limits<T>::lowest())
    {
    }

    constexpr explicit Bounds3(Point3<T> p) noexcept :
        m_min(p),
        m_max(p)
    {
    }

    constexpr Bounds3(Point3<T> p1, Point3<T> p2) noexcept :
        m_min(Min(p1, p2)),
        m_max(Max(p1, p2))
    {
    }

    [[nodiscard]] static constexpr Bounds3 FromPoints(Point3<T> a, Point3<T> b) noexcept
    {
        return Bounds3(a, b);
    }

    [[nodiscard]] static constexpr Bounds3 FromCenterHalfSize(Point3<T> center,
                                                              Vec3<T> halfSize) noexcept
    {
        return Bounds3(center - halfSize, center + halfSize);
    }

    // 0 = min corner, 1 = max corner.
    [[nodiscard]] constexpr Point3<T> operator[](int i) const noexcept
    {
        assert(i == 0 || i == 1);
        return (i == 0) ? m_min : m_max;
    }

    [[nodiscard]] constexpr Point3<T>& operator[](int i) noexcept
    {
        assert(i == 0 || i == 1);
        return (i == 0) ? m_min : m_max;
    }

    [[nodiscard]] constexpr Vec3<T> Diagonal() const noexcept { return m_max - m_min; }

    [[nodiscard]] constexpr Point3<T> Center() const noexcept
    {
        if constexpr (std::floating_point<T>)
        {
            return (m_min + m_max) * T(0.5);
        }
        else
        {
            return (m_min + m_max) / T(2);
        }
    }

    [[nodiscard]] constexpr Vec3<T> Size() const noexcept { return Diagonal(); }

    [[nodiscard]] constexpr T Volume() const noexcept
    {
        if (IsEmpty()) [[unlikely]]
        {
            return T(0);
        }
        return Prod(Diagonal());
    }

    [[nodiscard]] constexpr T SurfaceArea() const noexcept
    {
        if (IsEmpty()) [[unlikely]]
        {
            return T(0);
        }
        const Vec3<T> d = Diagonal();
        return T(2) * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    [[nodiscard]] constexpr bool IsEmpty() const noexcept
    {
        return m_min.m_pos.x >= m_max.m_pos.x || m_min.m_pos.y >= m_max.m_pos.y ||
               m_min.m_pos.z >= m_max.m_pos.z;
    }

    [[nodiscard]] constexpr bool IsDegenerate() const noexcept
    {
        return m_min.m_pos.x > m_max.m_pos.x || m_min.m_pos.y > m_max.m_pos.y ||
               m_min.m_pos.z > m_max.m_pos.z;
    }

    [[nodiscard]] constexpr int MaxDimensionIndex() const noexcept
    {
        const Vec3<T> d = Diagonal();
        if (d.x > d.y && d.x > d.z)
        {
            return 0;
        }
        return (d.y > d.z) ? 1 : 2;
    }

    // corner in [0, 7] selects one of the eight corners.
    [[nodiscard]] constexpr Point3<T> GetCorner(int corner) const noexcept
    {
        assert(corner >= 0 && corner < 8);
        return Point3<T>((*this)[corner & 1].m_pos.x, (*this)[(corner & 2) ? 1 : 0].m_pos.y,
                         (*this)[(corner & 4) ? 1 : 0].m_pos.z);
    }

    // Point at parametric coordinates t in [0, 1] per axis.
    [[nodiscard]] constexpr Point3<T> Lerp(Vec3<T> t) const noexcept
    {
        return Point3<T>(glm::mix(m_min.m_pos.x, m_max.m_pos.x, t.x),
                         glm::mix(m_min.m_pos.y, m_max.m_pos.y, t.y),
                         glm::mix(m_min.m_pos.z, m_max.m_pos.z, t.z));
    }

    // Maps p to parametric coordinates in [0, 1] per axis (unchanged on zero-width axes).
    [[nodiscard]] constexpr Vec3<T> NormalizedOffset(Point3<T> p) const noexcept
    {
        Vec3<T> o = p - m_min;
        if (m_max.m_pos.x > m_min.m_pos.x) [[likely]]
        {
            o.x /= m_max.m_pos.x - m_min.m_pos.x;
        }
        if (m_max.m_pos.y > m_min.m_pos.y) [[likely]]
        {
            o.y /= m_max.m_pos.y - m_min.m_pos.y;
        }
        if (m_max.m_pos.z > m_min.m_pos.z) [[likely]]
        {
            o.z /= m_max.m_pos.z - m_min.m_pos.z;
        }
        return o;
    }

    [[nodiscard]] constexpr bool Contains(Point3<T> p) const noexcept { return Inside(p, *this); }

    [[nodiscard]] constexpr bool Intersects(Bounds3 other) const noexcept
    {
        return Overlaps(*this, other);
    }

    [[nodiscard]] constexpr Bounds3 Union(Bounds3 other) const noexcept
    {
        return Bounds3(Min(m_min, other.m_min), Max(m_max, other.m_max));
    }

    constexpr Bounds3& Expand(Point3<T> p) noexcept
    {
        m_min = Min(m_min, p);
        m_max = Max(m_max, p);
        return *this;
    }

    constexpr Bounds3& Expand(Bounds3 other) noexcept
    {
        m_min = Min(m_min, other.m_min);
        m_max = Max(m_max, other.m_max);
        return *this;
    }
}; // struct Bounds3

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Bounds2<T> Union(const Bounds2<T>& bounds, Point2<T> p) noexcept
{
    Bounds2<T> result = bounds;
    result.Expand(p);
    return result;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Bounds2<T> Union(const Bounds2<T>& a, const Bounds2<T>& b) noexcept
{
    Bounds2<T> result;
    result.m_min = Min(a.m_min, b.m_min);
    result.m_max = Max(a.m_max, b.m_max);
    return result;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Bounds2<T> Intersect(const Bounds2<T>& a, const Bounds2<T>& b) noexcept
{
    Bounds2<T> result;
    result.m_min = Max(a.m_min, b.m_min);
    result.m_max = Min(a.m_max, b.m_max);
    return result;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool Overlaps(const Bounds2<T>& a, const Bounds2<T>& b) noexcept
{
    return a.m_max.m_pos.x >= b.m_min.m_pos.x && a.m_min.m_pos.x <= b.m_max.m_pos.x &&
           a.m_max.m_pos.y >= b.m_min.m_pos.y && a.m_min.m_pos.y <= b.m_max.m_pos.y;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool Inside(Point2<T> p, const Bounds2<T>& bounds) noexcept
{
    return p.m_pos.x >= bounds.m_min.m_pos.x && p.m_pos.x <= bounds.m_max.m_pos.x &&
           p.m_pos.y >= bounds.m_min.m_pos.y && p.m_pos.y <= bounds.m_max.m_pos.y;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool InsideExclusive(Point2<T> p, const Bounds2<T>& bounds) noexcept
{
    return p.m_pos.x >= bounds.m_min.m_pos.x && p.m_pos.x < bounds.m_max.m_pos.x &&
           p.m_pos.y >= bounds.m_min.m_pos.y && p.m_pos.y < bounds.m_max.m_pos.y;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point2<T> ClosestPoint(Point2<T> p, const Bounds2<T>& bounds) noexcept
{
    return Point2<T>(glm::clamp(p.m_pos.x, bounds.m_min.m_pos.x, bounds.m_max.m_pos.x),
                     glm::clamp(p.m_pos.y, bounds.m_min.m_pos.y, bounds.m_max.m_pos.y));
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Point3<T> ClosestPoint(Point3<T> p, const Bounds3<T>& bounds) noexcept
{
    return Point3<T>(glm::clamp(p.m_pos.x, bounds.m_min.m_pos.x, bounds.m_max.m_pos.x),
                     glm::clamp(p.m_pos.y, bounds.m_min.m_pos.y, bounds.m_max.m_pos.y),
                     glm::clamp(p.m_pos.z, bounds.m_min.m_pos.z, bounds.m_max.m_pos.z));
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T DistanceSquared(Point2<T> p, const Bounds2<T>& bounds) noexcept
{
    if (bounds.IsEmpty()) [[unlikely]]
    {
        if constexpr (std::floating_point<T>)
        {
            return std::numeric_limits<T>::infinity();
        }
        else
        {
            return std::numeric_limits<T>::max();
        }
    }
    return DistanceSquared(p, ClosestPoint(p, bounds));
}

template <std::floating_point T>
[[nodiscard]] inline T Distance(Point2<T> p, const Bounds2<T>& bounds) noexcept
{
    return std::sqrt(DistanceSquared(p, bounds));
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Bounds3<T> Union(const Bounds3<T>& bounds, Point3<T> p) noexcept
{
    Bounds3<T> result = bounds;
    result.Expand(p);
    return result;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Bounds3<T> Union(const Bounds3<T>& a, const Bounds3<T>& b) noexcept
{
    Bounds3<T> result;
    result.m_min = Min(a.m_min, b.m_min);
    result.m_max = Max(a.m_max, b.m_max);
    return result;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr Bounds3<T> Intersect(const Bounds3<T>& a, const Bounds3<T>& b) noexcept
{
    Bounds3<T> result;
    result.m_min = Max(a.m_min, b.m_min);
    result.m_max = Min(a.m_max, b.m_max);
    return result;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool Overlaps(const Bounds3<T>& a, const Bounds3<T>& b) noexcept
{
    return a.m_max.m_pos.x >= b.m_min.m_pos.x && a.m_min.m_pos.x <= b.m_max.m_pos.x &&
           a.m_max.m_pos.y >= b.m_min.m_pos.y && a.m_min.m_pos.y <= b.m_max.m_pos.y &&
           a.m_max.m_pos.z >= b.m_min.m_pos.z && a.m_min.m_pos.z <= b.m_max.m_pos.z;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool Inside(Point3<T> p, const Bounds3<T>& bounds) noexcept
{
    return p.m_pos.x >= bounds.m_min.m_pos.x && p.m_pos.x <= bounds.m_max.m_pos.x &&
           p.m_pos.y >= bounds.m_min.m_pos.y && p.m_pos.y <= bounds.m_max.m_pos.y &&
           p.m_pos.z >= bounds.m_min.m_pos.z && p.m_pos.z <= bounds.m_max.m_pos.z;
}

template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr bool InsideExclusive(Point3<T> p, const Bounds3<T>& bounds) noexcept
{
    return p.m_pos.x >= bounds.m_min.m_pos.x && p.m_pos.x < bounds.m_max.m_pos.x &&
           p.m_pos.y >= bounds.m_min.m_pos.y && p.m_pos.y < bounds.m_max.m_pos.y &&
           p.m_pos.z >= bounds.m_min.m_pos.z && p.m_pos.z < bounds.m_max.m_pos.z;
}

// Squared distance from p to the closest point on bounds (zero when p is inside).
template <typename T>
    requires std::is_arithmetic_v<T>
[[nodiscard]] constexpr T DistanceSquared(Point3<T> p, const Bounds3<T>& bounds) noexcept
{
    if (bounds.IsEmpty()) [[unlikely]]
    {
        if constexpr (std::floating_point<T>)
        {
            return std::numeric_limits<T>::infinity();
        }
        else
        {
            return std::numeric_limits<T>::max();
        }
    }
    return DistanceSquared(p, ClosestPoint(p, bounds));
}

template <std::floating_point T>
[[nodiscard]] inline T Distance(Point3<T> p, const Bounds3<T>& bounds) noexcept
{
    return std::sqrt(DistanceSquared(p, bounds));
}

// ---------------------------------------------------------------------------
// Shapes
// ---------------------------------------------------------------------------

template <std::floating_point T>
struct Circle
{
    Point2<T> m_center{};
    T m_radius{};

    constexpr Circle() noexcept = default;
    constexpr Circle(Point2<T> center, T radius) noexcept :
        m_center(center),
        m_radius(radius)
    {
        assert(m_radius >= T(0));
    }

    [[nodiscard]] constexpr T Area() const noexcept { return T(M_PI) * m_radius * m_radius; }

    [[nodiscard]] constexpr T Perimeter() const noexcept { return T(2) * T(M_PI) * m_radius; }

    [[nodiscard]] constexpr bool Contains(Point2<T> p) const noexcept
    {
        assert(m_radius >= T(0));
        return DistanceSquared(m_center, p) <= m_radius * m_radius;
    }

    [[nodiscard]] constexpr bool Intersects(Circle other) const noexcept
    {
        assert(m_radius >= T(0) && other.m_radius >= T(0));
        const T combinedRadius = m_radius + other.m_radius;
        return DistanceSquared(m_center, other.m_center) <= combinedRadius * combinedRadius;
    }

    [[nodiscard]] constexpr bool Intersects(Bounds2<T> bounds) const noexcept
    {
        assert(m_radius >= T(0));
        return DistanceSquared(m_center, bounds) <= m_radius * m_radius;
    }
}; // struct Circle

template <std::floating_point T>
[[nodiscard]] constexpr bool Intersects(const Circle<T>& circle, const Bounds2<T>& bounds) noexcept
{
    assert(circle.m_radius >= T(0));
    return DistanceSquared(circle.m_center, bounds) <= circle.m_radius * circle.m_radius;
}

template <std::floating_point T>
struct Sphere
{
    Point3<T> m_center{};
    T m_radius{};

    constexpr Sphere() noexcept = default;
    constexpr Sphere(Point3<T> center, T radius) noexcept :
        m_center(center),
        m_radius(radius)
    {
        assert(m_radius >= T(0));
    }

    [[nodiscard]] constexpr T Volume() const noexcept
    {
        return T(4) / T(3) * T(M_PI) * m_radius * m_radius * m_radius;
    }

    [[nodiscard]] constexpr T SurfaceArea() const noexcept
    {
        return T(4) * T(M_PI) * m_radius * m_radius;
    }

    [[nodiscard]] constexpr bool Contains(Point3<T> p) const noexcept
    {
        assert(m_radius >= T(0));
        return DistanceSquared(m_center, p) <= m_radius * m_radius;
    }

    [[nodiscard]] constexpr bool Intersects(Sphere other) const noexcept
    {
        assert(m_radius >= T(0) && other.m_radius >= T(0));
        const T combinedRadius = m_radius + other.m_radius;
        return DistanceSquared(m_center, other.m_center) <= combinedRadius * combinedRadius;
    }

    [[nodiscard]] constexpr bool Intersects(Bounds3<T> bounds) const noexcept
    {
        assert(m_radius >= T(0));
        return DistanceSquared(m_center, bounds) <= m_radius * m_radius;
    }
}; // struct Sphere

template <std::floating_point T>
[[nodiscard]] constexpr bool Intersects(const Sphere<T>& sphere, const Bounds3<T>& bounds) noexcept
{
    assert(sphere.m_radius >= T(0));
    return DistanceSquared(sphere.m_center, bounds) <= sphere.m_radius * sphere.m_radius;
}

template <std::floating_point T>
struct LineSegment2
{
    Point2<T> m_a{};
    Point2<T> m_b{};

    constexpr LineSegment2() noexcept = default;
    constexpr LineSegment2(Point2<T> a, Point2<T> b) noexcept :
        m_a(a),
        m_b(b)
    {
    }

    [[nodiscard]] constexpr T LengthSquared() const noexcept { return DistanceSquared(m_a, m_b); }

    [[nodiscard]] inline T Length() const noexcept { return Distance(m_a, m_b); }

    [[nodiscard]] constexpr Point2<T> Center() const noexcept { return (m_a + m_b) * T(0.5); }

    [[nodiscard]] inline Vec2<T> Direction() const noexcept { return Normalize(m_b - m_a); }
}; // struct LineSegment2

template <std::floating_point T>
struct LineSegment3
{
    Point3<T> m_a{};
    Point3<T> m_b{};

    constexpr LineSegment3() noexcept = default;
    constexpr LineSegment3(Point3<T> a, Point3<T> b) noexcept :
        m_a(a),
        m_b(b)
    {
    }

    [[nodiscard]] constexpr T LengthSquared() const noexcept { return DistanceSquared(m_a, m_b); }

    [[nodiscard]] inline T Length() const noexcept { return Distance(m_a, m_b); }

    [[nodiscard]] constexpr Point3<T> Center() const noexcept { return (m_a + m_b) * T(0.5); }

    [[nodiscard]] inline Vec3<T> Direction() const noexcept { return Normalize(m_b - m_a); }
}; // struct LineSegment3

using OrthoFrame2f = OrthoFrame2<float>;
using OrthoFrame2d = OrthoFrame2<double>;
using OrthoFrame3f = OrthoFrame3<float>;
using OrthoFrame3d = OrthoFrame3<double>;
using Bounds2f = Bounds2<float>;
using Bounds2d = Bounds2<double>;
using Bounds2i = Bounds2<int>;
using Bounds3f = Bounds3<float>;
using Bounds3d = Bounds3<double>;
using Bounds3i = Bounds3<int>;
using Circlef = Circle<float>;
using Circled = Circle<double>;
using Spheref = Sphere<float>;
using Sphered = Sphere<double>;
using LineSegment2f = LineSegment2<float>;
using LineSegment2d = LineSegment2<double>;
using LineSegment3f = LineSegment3<float>;
using LineSegment3d = LineSegment3<double>;

} // namespace rad
