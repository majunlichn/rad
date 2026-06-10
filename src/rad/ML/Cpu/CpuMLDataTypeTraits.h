#pragma once

#include <rad/Common/BFloat16.h>
#include <rad/Common/Float16.h>
#include <rad/ML/MLCommon.h>

#include <cstdint>

namespace rad
{

template <MLDataType DataType>
struct CpuMLDataTypeTraits;

template <>
struct CpuMLDataTypeTraits<MLDataType::Float32>
{
    using StorageT = float;
    using ComputeT = float;
}; // struct CpuMLDataTypeTraits<MLDataType::Float32>

template <>
struct CpuMLDataTypeTraits<MLDataType::Float64>
{
    using StorageT = double;
    using ComputeT = double;
}; // struct CpuMLDataTypeTraits<MLDataType::Float64>

template <>
struct CpuMLDataTypeTraits<MLDataType::Float16>
{
    using StorageT = Float16;
    using ComputeT = float;
}; // struct CpuMLDataTypeTraits<MLDataType::Float16>

template <>
struct CpuMLDataTypeTraits<MLDataType::BFloat16>
{
    using StorageT = BFloat16;
    using ComputeT = float;
}; // struct CpuMLDataTypeTraits<MLDataType::BFloat16>

template <>
struct CpuMLDataTypeTraits<MLDataType::Int8>
{
    using StorageT = std::int8_t;
    using ComputeT = std::int8_t;
}; // struct CpuMLDataTypeTraits<MLDataType::Int8>

template <>
struct CpuMLDataTypeTraits<MLDataType::Int16>
{
    using StorageT = std::int16_t;
    using ComputeT = std::int16_t;
}; // struct CpuMLDataTypeTraits<MLDataType::Int16>

template <>
struct CpuMLDataTypeTraits<MLDataType::Int32>
{
    using StorageT = std::int32_t;
    using ComputeT = std::int32_t;
}; // struct CpuMLDataTypeTraits<MLDataType::Int32>

template <>
struct CpuMLDataTypeTraits<MLDataType::Int64>
{
    using StorageT = std::int64_t;
    using ComputeT = std::int64_t;
}; // struct CpuMLDataTypeTraits<MLDataType::Int64>

template <>
struct CpuMLDataTypeTraits<MLDataType::Uint8>
{
    using StorageT = std::uint8_t;
    using ComputeT = std::uint8_t;
}; // struct CpuMLDataTypeTraits<MLDataType::Uint8>

template <>
struct CpuMLDataTypeTraits<MLDataType::Uint16>
{
    using StorageT = std::uint16_t;
    using ComputeT = std::uint16_t;
}; // struct CpuMLDataTypeTraits<MLDataType::Uint16>

template <>
struct CpuMLDataTypeTraits<MLDataType::Uint32>
{
    using StorageT = std::uint32_t;
    using ComputeT = std::uint32_t;
}; // struct CpuMLDataTypeTraits<MLDataType::Uint32>

template <>
struct CpuMLDataTypeTraits<MLDataType::Uint64>
{
    using StorageT = std::uint64_t;
    using ComputeT = std::uint64_t;
}; // struct CpuMLDataTypeTraits<MLDataType::Uint64>

template <>
struct CpuMLDataTypeTraits<MLDataType::Bool>
{
    using StorageT = bool;
    using ComputeT = bool;
}; // struct CpuMLDataTypeTraits<MLDataType::Bool>

template <MLDataType... DataTypes>
struct CpuMLSupportedDataTypes
{
}; // struct CpuMLSupportedDataTypes

using CpuMLFloatingPointDataTypes =
    CpuMLSupportedDataTypes<MLDataType::Float16, MLDataType::BFloat16, MLDataType::Float32,
                            MLDataType::Float64>;

using CpuMLSignedIntegralDataTypes = CpuMLSupportedDataTypes<MLDataType::Int8, MLDataType::Int16,
                                                             MLDataType::Int32, MLDataType::Int64>;

using CpuMLUnsignedIntegralDataTypes =
    CpuMLSupportedDataTypes<MLDataType::Uint8, MLDataType::Uint16, MLDataType::Uint32,
                            MLDataType::Uint64>;

using CpuMLBoolDataTypes = CpuMLSupportedDataTypes<MLDataType::Bool>;

} // namespace rad
