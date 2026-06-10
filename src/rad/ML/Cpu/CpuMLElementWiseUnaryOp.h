#pragma once

#include <rad/ML/Cpu/CpuMLDataTypeTraits.h>
#include <rad/ML/Cpu/CpuMLDevice.h>
#include <rad/ML/MLOp.h>
#include <rad/ML/MLTensor.h>

#include <cstring>
#include <format>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace rad
{

// Element-wise unary op on CPU tensors. UnaryFn must provide GetName(), operator()(T), and
// SupportedDataTypes. Input/output tensors are bound at Execute time so the same op can run on
// varying shapes.
template <typename UnaryFn>
class CpuMLElementWiseUnaryOp final : public MLOp
{
public:
    CpuMLElementWiseUnaryOp(Ref<CpuMLDevice> device, UnaryFn unaryFn = {}) :
        MLOp(device),
        m_unaryFn(std::move(unaryFn))
    {
    }

    [[nodiscard]] const char* GetName() const noexcept override { return UnaryFn::GetName(); }

    void SetParameters(const void* data, size_t dataSize) override
    {
        if constexpr (requires { typename UnaryFn::Parameters; })
        {
            using Parameters = typename UnaryFn::Parameters;
            if (dataSize != sizeof(Parameters))
            {
                throw std::invalid_argument(
                    std::format("CpuMLElementWiseUnaryOp {} requires {} parameter bytes, got {}",
                                UnaryFn::GetName(), sizeof(Parameters), dataSize));
            }

            if (!data)
            {
                throw std::invalid_argument(
                    std::format("CpuMLElementWiseUnaryOp {} requires valid parameter data",
                                UnaryFn::GetName()));
            }

            Parameters parameters;
            std::memcpy(&parameters, data, sizeof(Parameters));
            m_unaryFn.SetParameters(parameters);
        }
        else
        {
            MLOp::SetParameters(data, dataSize);
        }
    }

    void Execute(MLContext* context, Span<MLTensor* const> inputs,
                 Span<MLTensor* const> outputs) override
    {
        (void)context;

        if (inputs.size() != 1 || outputs.size() != 1)
        {
            throw std::invalid_argument(std::format(
                "CpuMLElementWiseUnaryOp {} requires exactly one input and one output", GetName()));
        }

        MLTensor* input = inputs[0];
        MLTensor* output = outputs[0];
        if (!input || !output)
        {
            throw std::invalid_argument(std::format(
                "CpuMLElementWiseUnaryOp {} requires valid input and output tensors", GetName()));
        }

        ValidateTensors(input, output, GetDevice());

        Dispatch(input, output, m_unaryFn);
    }

private:
    static void ValidateTensors(MLTensor* input, MLTensor* output, MLDevice* opDevice)
    {
        if (!opDevice)
        {
            throw std::invalid_argument(std::format(
                "CpuMLElementWiseUnaryOp {} requires a valid device", UnaryFn::GetName()));
        }

        if (input->GetDevice() != opDevice || output->GetDevice() != opDevice)
        {
            throw std::invalid_argument(
                std::format("CpuMLElementWiseUnaryOp {} requires input and output on the op device",
                            UnaryFn::GetName()));
        }

        const MLTensorDesc& inputDesc = input->GetDesc();
        const MLTensorDesc& outputDesc = output->GetDesc();

        if (inputDesc.shape != outputDesc.shape)
        {
            throw std::invalid_argument(
                std::format("CpuMLElementWiseUnaryOp {} requires matching input and output shapes",
                            UnaryFn::GetName()));
        }

        if (inputDesc.dataType != outputDesc.dataType)
        {
            throw std::invalid_argument(std::format(
                "CpuMLElementWiseUnaryOp {} requires matching input and output data types",
                UnaryFn::GetName()));
        }

        if (!ValidateTensorDesc(inputDesc))
        {
            throw std::invalid_argument(
                std::format("CpuMLElementWiseUnaryOp {} requires a valid input tensor desc",
                            UnaryFn::GetName()));
        }

        if (!ValidateTensorDesc(outputDesc))
        {
            throw std::invalid_argument(
                std::format("CpuMLElementWiseUnaryOp {} requires a valid output tensor desc",
                            UnaryFn::GetName()));
        }

        if (HasOverlappingStorageOffsets(outputDesc))
        {
            throw std::invalid_argument(std::format(
                "CpuMLElementWiseUnaryOp {} requires a non-overlapping output tensor view",
                UnaryFn::GetName()));
        }
    }

    static bool HasOverlappingStorageOffsets(const MLTensorDesc& desc)
    {
        if (IsTensorContiguous(desc))
        {
            return false;
        }

        const MLSizes strides = GetTensorStrides(desc);
        const size_t rank = desc.shape.size();
        const size_t elementCount = GetTensorElementCount(desc);

        std::unordered_set<size_t> offsets;
        offsets.reserve(elementCount);

        MLSizes indices(rank, 0);
        for (size_t n = 0; n < elementCount; ++n)
        {
            size_t offset = 0;
            for (size_t axis = 0; axis < rank; ++axis)
            {
                offset += indices[axis] * strides[axis];
            }

            if (!offsets.insert(offset).second)
            {
                return true;
            }

            for (size_t axis = rank; axis-- > 0;)
            {
                if (++indices[axis] < desc.shape[axis])
                {
                    break;
                }

                indices[axis] = 0;
                if (axis == 0)
                {
                    break;
                }
            }
        }

        return false;
    }

    template <typename StorageT, typename ComputeT>
    static void DispatchContiguousImpl(const StorageT* input, StorageT* output, size_t elementCount,
                                       const UnaryFn& unaryFn)
    {
        for (size_t i = 0; i < elementCount; ++i)
        {
            output[i] = static_cast<StorageT>(unaryFn(static_cast<ComputeT>(input[i])));
        }
    }

    template <typename StorageT, typename ComputeT>
    static void DispatchStridedImpl(const MLTensorDesc& inputDesc, const StorageT* input,
                                    const MLTensorDesc& outputDesc, StorageT* output,
                                    const UnaryFn& unaryFn)
    {
        const MLSizes inputStrides = GetTensorStrides(inputDesc);
        const MLSizes outputStrides = GetTensorStrides(outputDesc);
        const size_t rank = inputDesc.shape.size();
        const size_t elementCount = GetTensorElementCount(inputDesc);

        MLSizes indices(rank, 0);
        for (size_t n = 0; n < elementCount; ++n)
        {
            size_t inputOffset = 0;
            size_t outputOffset = 0;
            for (size_t axis = 0; axis < rank; ++axis)
            {
                inputOffset += indices[axis] * inputStrides[axis];
                outputOffset += indices[axis] * outputStrides[axis];
            }

            output[outputOffset] =
                static_cast<StorageT>(unaryFn(static_cast<ComputeT>(input[inputOffset])));

            for (size_t axis = rank; axis-- > 0;)
            {
                if (++indices[axis] < inputDesc.shape[axis])
                {
                    break;
                }

                indices[axis] = 0;
                if (axis == 0)
                {
                    break;
                }
            }
        }
    }

    template <typename StorageT, typename ComputeT = StorageT>
    static void DispatchImpl(MLTensor* input, MLTensor* output, const UnaryFn& unaryFn)
    {
        const MLTensorDesc& inputDesc = input->GetDesc();
        const MLTensorDesc& outputDesc = output->GetDesc();

        StorageT* inputData = static_cast<StorageT*>(input->MapMemory());
        StorageT* outputData = static_cast<StorageT*>(output->MapMemory());

        try
        {
            if (input->IsContiguous() && output->IsContiguous())
            {
                DispatchContiguousImpl<StorageT, ComputeT>(inputData, outputData,
                                                           input->GetElementCount(), unaryFn);
            }
            else
            {
                DispatchStridedImpl<StorageT, ComputeT>(inputDesc, inputData, outputDesc,
                                                        outputData, unaryFn);
            }
        }
        catch (...)
        {
            input->UnmapMemory();
            output->UnmapMemory();
            throw;
        }

        input->UnmapMemory();
        output->UnmapMemory();
    }

    template <MLDataType... DataTypes>
    static void Dispatch(CpuMLSupportedDataTypes<DataTypes...>, MLTensor* input, MLTensor* output,
                         const UnaryFn& unaryFn)
    {
        const MLDataType dataType = input->GetDesc().dataType;
        const auto tryDispatch = [&]<MLDataType DataType>()
        {
            if (dataType != DataType)
            {
                return false;
            }

            using DataTypeTraits = CpuMLDataTypeTraits<DataType>;
            DispatchImpl<typename DataTypeTraits::StorageT, typename DataTypeTraits::ComputeT>(
                input, output, unaryFn);
            return true;
        };

        const bool dispatched = (tryDispatch.template operator()<DataTypes>() || ...);

        if (dispatched)
        {
            return;
        }

        throw std::invalid_argument(
            std::format("CpuMLElementWiseUnaryOp {} does not support tensor data type {}",
                        UnaryFn::GetName(), GetDataTypeName(dataType)));
    }

    static void Dispatch(MLTensor* input, MLTensor* output, const UnaryFn& unaryFn)
    {
        Dispatch(typename UnaryFn::SupportedDataTypes{}, input, output, unaryFn);
    }

    UnaryFn m_unaryFn;
}; // class CpuMLElementWiseUnaryOp

template <typename UnaryFn>
[[nodiscard]] Ref<MLOp> CpuMLCreateElementWiseUnaryOp(Ref<CpuMLDevice> device, UnaryFn unaryFn = {})
{
    return Ref<MLOp>(new CpuMLElementWiseUnaryOp<UnaryFn>(std::move(device), std::move(unaryFn)));
}

} // namespace rad
