#pragma once

#include <rad/Container/SmallVector.h>
#include <rad/IO/Logging.h>

#include <cstddef>
#include <cstdint>

#include <rad/Common/Flags.h>

namespace rad
{

spdlog::logger* GetMLLogger();

// LogLevel: trace, debug, info, warn, err, critical
#define RAD_LOG_ML(LogLevel, ...)                                                                  \
    SPDLOG_LOGGER_CALL(::rad::GetMLLogger(), spdlog::level::LogLevel, __VA_ARGS__)

enum class MLDataType : std::uint8_t
{
    Undefined,

    Float32,
    Float64,

    Int8,
    Int16,
    Int32,
    Int64,

    Uint8,
    Uint16,
    Uint32,
    Uint64,

    Bool,

    Float16,  // Exponent bits: 5, Mantissa bits: 10
    BFloat16, // Exponent bits: 8, Mantissa bits: 7
    Float8E4M3,
    Float8E5M2,
}; // enum class MLDataType

enum class MLDataTypeCategory : std::uint8_t
{
    FloatingPoint = 1u << 0,
    SignedIntegral = 1u << 1,
    UnsignedIntegral = 1u << 2,
    Bool = 1u << 3,
    All = 0xFF,
}; // enum class MLDataTypeCategory

RAD_FLAG_TRAITS(MLDataTypeCategory, MLDataTypeCategory::All);

[[nodiscard]] constexpr bool IsFloatingPoint(MLDataType dataType) noexcept
{
    switch (dataType)
    {
    case MLDataType::Float32:
    case MLDataType::Float64:
    case MLDataType::Float16:
    case MLDataType::BFloat16:
    case MLDataType::Float8E4M3:
    case MLDataType::Float8E5M2:
        return true;
    default:
        return false;
    }
}

[[nodiscard]] constexpr bool IsSignedIntegral(MLDataType dataType) noexcept
{
    switch (dataType)
    {
    case MLDataType::Int8:
    case MLDataType::Int16:
    case MLDataType::Int32:
    case MLDataType::Int64:
        return true;
    default:
        return false;
    }
}

[[nodiscard]] constexpr bool IsUnsignedIntegral(MLDataType dataType) noexcept
{
    switch (dataType)
    {
    case MLDataType::Uint8:
    case MLDataType::Uint16:
    case MLDataType::Uint32:
    case MLDataType::Uint64:
        return true;
    default:
        return false;
    }
}

[[nodiscard]] constexpr bool IsIntegral(MLDataType dataType) noexcept
{
    return IsSignedIntegral(dataType) || IsUnsignedIntegral(dataType) ||
           dataType == MLDataType::Bool;
}

[[nodiscard]] constexpr MLDataTypeCategory GetMLDataTypeCategory(MLDataType dataType) noexcept
{
    if (IsFloatingPoint(dataType))
    {
        return MLDataTypeCategory::FloatingPoint;
    }

    if (IsSignedIntegral(dataType))
    {
        return MLDataTypeCategory::SignedIntegral;
    }

    if (IsUnsignedIntegral(dataType))
    {
        return MLDataTypeCategory::UnsignedIntegral;
    }

    if (dataType == MLDataType::Bool)
    {
        return MLDataTypeCategory::Bool;
    }

    return static_cast<MLDataTypeCategory>(0);
}

using MLSizes = SmallVector<size_t, 4>;

// Shape, strides, and element type for a tensor view. Strides are in elements, not bytes.
struct MLTensorDesc
{
    MLSizes shape;
    // Empty strides: row-major contiguous layout. Zero stride: broadcast along that axis.
    MLSizes strides;
    MLDataType dataType = MLDataType::Float32;
}; // struct MLTensorDesc

[[nodiscard]] constexpr size_t GetDataTypeSize(MLDataType dataType) noexcept
{
    switch (dataType)
    {
    case MLDataType::Undefined:
        return 0;
    case MLDataType::Float32:
        return 4;
    case MLDataType::Float64:
        return 8;
    case MLDataType::Int8:
        return 1;
    case MLDataType::Int16:
        return 2;
    case MLDataType::Int32:
        return 4;
    case MLDataType::Int64:
        return 8;
    case MLDataType::Uint8:
        return 1;
    case MLDataType::Uint16:
        return 2;
    case MLDataType::Uint32:
        return 4;
    case MLDataType::Uint64:
        return 8;
    case MLDataType::Bool:
        return 1;
    case MLDataType::Float16:
    case MLDataType::BFloat16:
        return 2;
    case MLDataType::Float8E4M3:
    case MLDataType::Float8E5M2:
        return 1;
    default:
        return 0;
    }
}

[[nodiscard]] constexpr const char* GetDataTypeName(MLDataType dataType) noexcept
{
    switch (dataType)
    {
    case MLDataType::Undefined:
        return "Undefined";
    case MLDataType::Float32:
        return "Float32";
    case MLDataType::Float64:
        return "Float64";
    case MLDataType::Int8:
        return "Int8";
    case MLDataType::Int16:
        return "Int16";
    case MLDataType::Int32:
        return "Int32";
    case MLDataType::Int64:
        return "Int64";
    case MLDataType::Uint8:
        return "Uint8";
    case MLDataType::Uint16:
        return "Uint16";
    case MLDataType::Uint32:
        return "Uint32";
    case MLDataType::Uint64:
        return "Uint64";
    case MLDataType::Bool:
        return "Bool";
    case MLDataType::Float16:
        return "Float16";
    case MLDataType::BFloat16:
        return "BFloat16";
    case MLDataType::Float8E4M3:
        return "Float8E4M3";
    case MLDataType::Float8E5M2:
        return "Float8E5M2";
    default:
        return "Unknown";
    }
}

// Product of shape dimensions (numel).
[[nodiscard]] size_t GetTensorElementCount(const MLSizes& shape);
[[nodiscard]] size_t GetTensorElementCount(const MLTensorDesc& desc);

// Element count spanned in storage (>= numel for general strides).
[[nodiscard]] size_t GetTensorStorageElementCount(const MLSizes& shape, const MLSizes& strides);
[[nodiscard]] size_t GetTensorStorageElementCount(const MLTensorDesc& desc);

// Byte size of the storage span for desc.
[[nodiscard]] size_t GetTensorStorageSize(const MLTensorDesc& desc);

[[nodiscard]] MLSizes GetDefaultTensorStrides(MLSizes shape);
[[nodiscard]] MLSizes GetTensorStrides(const MLTensorDesc& desc);
[[nodiscard]] bool IsTensorContiguous(const MLTensorDesc& desc) noexcept;
[[nodiscard]] bool ValidateTensorDesc(const MLTensorDesc& desc) noexcept;

} // namespace rad
