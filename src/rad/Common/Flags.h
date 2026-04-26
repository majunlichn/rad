#pragma once

#include <rad/Common/Integer.h>

#include <rad/Common/TypeTraits.h>

namespace rad
{

template <Enumeration FlagBits>
struct FlagTraits
{
    static constexpr bool isBitmask = false;
    // Specializations should provide:
    // static constexpr Flags<FlagBits> allBits;
};

#define RAD_FLAG_TRAITS(FlagBits, All)                                                             \
    template <>                                                                                    \
    struct FlagTraits<FlagBits>                                                                    \
    {                                                                                              \
        static constexpr bool isBitmask = true;                                                    \
        static constexpr Flags<FlagBits> allBits = Flags<FlagBits>(All);                           \
    }

// Inspired by https://github.com/KhronosGroup/Vulkan-Hpp
template <Enumeration FlagBits>
class Flags
{
public:
    using MaskType = std::underlying_type_t<FlagBits>;

    // constructors
    constexpr Flags() noexcept :
        m_mask(0)
    {
    }

    constexpr Flags(FlagBits bits) noexcept :
        m_mask(static_cast<MaskType>(bits))
    {
    }

    constexpr Flags(Flags<FlagBits> const& rhs) noexcept = default;

    constexpr explicit Flags(MaskType flags) noexcept :
        m_mask(flags)
    {
    }

    [[nodiscard]] static constexpr Flags<FlagBits> FromMask(MaskType mask) noexcept
    {
        assert(FlagTraits<FlagBits>::allBits.HasAllBits(mask));
        return Flags<FlagBits>(mask);
    }

    [[nodiscard]] constexpr MaskType GetMask() const noexcept { return m_mask; }

    [[nodiscard]] constexpr bool HasAllBits(MaskType otherMask) const noexcept
    {
        return (m_mask & otherMask) == otherMask;
    }

    [[nodiscard]] constexpr bool HasAnyBits(MaskType otherMask) const noexcept
    {
        return (m_mask & otherMask) != 0;
    }

    [[nodiscard]] constexpr bool HasNoBits(MaskType otherMask) const noexcept
    {
        return (m_mask & otherMask) == 0;
    }

    auto operator<=>(Flags<FlagBits> const&) const = default;

    // logical operator
    constexpr bool operator!() const noexcept { return m_mask == 0; }

    // bitwise operators
    [[nodiscard]] constexpr Flags<FlagBits> operator&(Flags<FlagBits> const& rhs) const noexcept
    {
        return Flags<FlagBits>(m_mask & rhs.m_mask);
    }

    [[nodiscard]] constexpr Flags<FlagBits> operator|(Flags<FlagBits> const& rhs) const noexcept
    {
        return Flags<FlagBits>(m_mask | rhs.m_mask);
    }

    [[nodiscard]] constexpr Flags<FlagBits> operator^(Flags<FlagBits> const& rhs) const noexcept
    {
        return Flags<FlagBits>(m_mask ^ rhs.m_mask);
    }

    [[nodiscard]] constexpr Flags<FlagBits> operator~() const noexcept
    {
        return Flags<FlagBits>(m_mask ^ FlagTraits<FlagBits>::allBits.GetMask());
    }

    // assignment operators
    constexpr Flags<FlagBits>& operator=(Flags<FlagBits> const& rhs) noexcept = default;

    constexpr Flags<FlagBits>& operator|=(Flags<FlagBits> const& rhs) noexcept
    {
        m_mask |= rhs.m_mask;
        return *this;
    }

    constexpr Flags<FlagBits>& operator&=(Flags<FlagBits> const& rhs) noexcept
    {
        m_mask &= rhs.m_mask;
        return *this;
    }

    constexpr Flags<FlagBits>& operator^=(Flags<FlagBits> const& rhs) noexcept
    {
        m_mask ^= rhs.m_mask;
        return *this;
    }

    // cast operators
    explicit constexpr operator bool() const noexcept { return !!m_mask; }

    explicit constexpr operator MaskType() const noexcept { return m_mask; }

private:
    MaskType m_mask = 0;

}; // class Flags

// bitwise operators
template <Enumeration FlagBits>
constexpr Flags<FlagBits> operator&(FlagBits bits, Flags<FlagBits> const& flags) noexcept
{
    return flags.operator&(bits);
}

template <Enumeration FlagBits>
constexpr Flags<FlagBits> operator|(FlagBits bits, Flags<FlagBits> const& flags) noexcept
{
    return flags.operator|(bits);
}

template <Enumeration FlagBits>
constexpr Flags<FlagBits> operator^(FlagBits bits, Flags<FlagBits> const& flags) noexcept
{
    return flags.operator^(bits);
}

// bitwise operators on FlagBits
template <typename FlagBits>
    requires FlagTraits<FlagBits>::isBitmask
constexpr Flags<FlagBits> operator&(FlagBits lhs, FlagBits rhs) noexcept
{
    return Flags<FlagBits>(lhs) & rhs;
}

template <typename FlagBits>
    requires FlagTraits<FlagBits>::isBitmask
constexpr Flags<FlagBits> operator|(FlagBits lhs, FlagBits rhs) noexcept
{
    return Flags<FlagBits>(lhs) | rhs;
}

template <typename FlagBits>
    requires FlagTraits<FlagBits>::isBitmask
constexpr Flags<FlagBits> operator^(FlagBits lhs, FlagBits rhs) noexcept
{
    return Flags<FlagBits>(lhs) ^ rhs;
}

template <typename FlagBits>
    requires FlagTraits<FlagBits>::isBitmask
constexpr Flags<FlagBits> operator~(FlagBits bits) noexcept
{
    return ~(Flags<FlagBits>(bits));
}

} // namespace rad
