#pragma once

#include <rad/ML/Cpu/CpuMLElementWiseUnaryOp.h>
#include <rad/ML/MLOp.h>
#include <rad/ML/MLTensor.h>

#include <algorithm>
#include <cmath>

namespace rad
{

namespace CpuMLElementWiseUnaryFn
{

struct Relu
{
    using SupportedDataTypes = CpuMLFloatingPointDataTypes;

    static constexpr const char* GetName() noexcept { return "Relu"; }

    template <typename T>
    T operator()(T x) const noexcept
    {
        return std::max(T{0}, x);
    }
}; // struct Relu

struct LeakyRelu
{
    struct Parameters
    {
        float alpha = 0.01f;
    }; // struct Parameters

    using SupportedDataTypes = CpuMLFloatingPointDataTypes;

    float alpha = 0.01f;

    static constexpr const char* GetName() noexcept { return "LeakyRelu"; }

    void SetParameters(const Parameters& parameters) noexcept { alpha = parameters.alpha; }

    template <typename T>
    T operator()(T x) const noexcept
    {
        return x > T{0} ? x : static_cast<T>(alpha) * x;
    }
}; // struct LeakyRelu

struct Sigmoid
{
    using SupportedDataTypes = CpuMLFloatingPointDataTypes;

    static constexpr const char* GetName() noexcept { return "Sigmoid"; }

    template <typename T>
    T operator()(T x) const noexcept
    {
        if (x >= T{0})
        {
            const T one = T{1};
            const T expNegX = std::exp(-x);
            return one / (one + expNegX);
        }

        const T expX = std::exp(x);
        return expX / (T{1} + expX);
    }
}; // struct Sigmoid

struct Tanh
{
    using SupportedDataTypes = CpuMLFloatingPointDataTypes;

    static constexpr const char* GetName() noexcept { return "Tanh"; }

    template <typename T>
    T operator()(T x) const noexcept
    {
        return std::tanh(x);
    }
}; // struct Tanh

// ONNX Gelu with approximate=0.
struct Gelu
{
    using SupportedDataTypes = CpuMLFloatingPointDataTypes;

    static constexpr const char* GetName() noexcept { return "Gelu"; }

    template <typename T>
    T operator()(T x) const noexcept
    {
        using std::erf;
        return T{0.5} * x * (T{1} + erf(x / std::sqrt(T{2})));
    }
}; // struct Gelu

// ONNX Gelu with approximate=1 (tanh approximation).
struct FastGelu
{
    using SupportedDataTypes = CpuMLFloatingPointDataTypes;

    static constexpr const char* GetName() noexcept { return "FastGelu"; }

    template <typename T>
    T operator()(T x) const noexcept
    {
        const T x3 = x * x * x;
        const T inner = std::sqrt(T{2} / T{3.14159265358979323846}) * (x + T{0.044715} * x3);
        return T{0.5} * x * (T{1} + std::tanh(inner));
    }
}; // struct FastGelu

struct Silu
{
    using SupportedDataTypes = CpuMLFloatingPointDataTypes;

    static constexpr const char* GetName() noexcept { return "Silu"; }

    template <typename T>
    T operator()(T x) const noexcept
    {
        return x * Sigmoid{}(x);
    }
}; // struct Silu

struct Softplus
{
    using SupportedDataTypes = CpuMLFloatingPointDataTypes;

    static constexpr const char* GetName() noexcept { return "Softplus"; }

    template <typename T>
    T operator()(T x) const noexcept
    {
        // log(1 + exp(x)) in a numerically stable form.
        if (x > T{20})
        {
            return x;
        }

        return std::log1p(std::exp(x));
    }
}; // struct Softplus

struct Elu
{
    struct Parameters
    {
        float alpha = 1.0f;
    }; // struct Parameters

    using SupportedDataTypes = CpuMLFloatingPointDataTypes;

    float alpha = 1.0f;

    static constexpr const char* GetName() noexcept { return "Elu"; }

    void SetParameters(const Parameters& parameters) noexcept { alpha = parameters.alpha; }

    template <typename T>
    T operator()(T x) const noexcept
    {
        return x > T{0} ? x : static_cast<T>(alpha) * (std::exp(x) - T{1});
    }
}; // struct Elu

} // namespace CpuMLElementWiseUnaryFn

[[nodiscard]] Ref<MLOp> CpuMLCreateReluOp(Ref<CpuMLDevice> device);
[[nodiscard]] Ref<MLOp> CpuMLCreateLeakyReluOp(Ref<CpuMLDevice> device, float alpha = 0.01f);
[[nodiscard]] Ref<MLOp> CpuMLCreateSigmoidOp(Ref<CpuMLDevice> device);
[[nodiscard]] Ref<MLOp> CpuMLCreateTanhOp(Ref<CpuMLDevice> device);
[[nodiscard]] Ref<MLOp> CpuMLCreateGeluOp(Ref<CpuMLDevice> device);
[[nodiscard]] Ref<MLOp> CpuMLCreateFastGeluOp(Ref<CpuMLDevice> device);
[[nodiscard]] Ref<MLOp> CpuMLCreateSiluOp(Ref<CpuMLDevice> device);
[[nodiscard]] Ref<MLOp> CpuMLCreateSoftplusOp(Ref<CpuMLDevice> device);
[[nodiscard]] Ref<MLOp> CpuMLCreateEluOp(Ref<CpuMLDevice> device, float alpha = 1.0f);

} // namespace rad
