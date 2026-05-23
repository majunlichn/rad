#include <rad/ML/MLCommon.h>

#include <cassert>

namespace rad
{

spdlog::logger* GetMLLogger()
{
    static std::shared_ptr<spdlog::logger> logger = CreateLogger("ML");
    return logger.get();
}

size_t GetTensorElementCount(const MLSizes& shape)
{
    if (shape.empty())
    {
        return 0;
    }

    size_t count = 1;
    for (const size_t dim : shape)
    {
        count *= dim;
    }
    return count;
}

size_t GetTensorElementCount(const MLTensorDesc& desc)
{
    return GetTensorElementCount(desc.shape);
}

MLSizes GetDefaultTensorStrides(MLSizes shape)
{
    MLSizes strides;
    if (shape.empty())
    {
        return strides;
    }

    strides.resize(shape.size());
    size_t stride = 1;
    for (size_t i = shape.size(); i-- > 0;)
    {
        strides[i] = stride;
        stride *= shape[i];
    }
    return strides;
}

MLSizes GetTensorStrides(const MLTensorDesc& desc)
{
    if (desc.strides.empty())
    {
        return GetDefaultTensorStrides(desc.shape);
    }

    assert(desc.strides.size() == desc.shape.size());
    return desc.strides;
}

size_t GetTensorStorageElementCount(const MLSizes& shape, const MLSizes& strides)
{
    if (shape.empty())
    {
        return 0;
    }

    for (const size_t dim : shape)
    {
        if (dim == 0)
        {
            return 0;
        }
    }

    if (strides.empty())
    {
        return GetTensorElementCount(shape);
    }

    assert(strides.size() == shape.size());

    // Storage element span: max linear index + 1 for indices with 0 <= index[i] < shape[i].
    size_t storageElements = 1;
    for (size_t i = 0; i < shape.size(); ++i)
    {
        if (strides[i] == 0)
        {
            assert((shape[i] - 1) * strides[i] == 0);
        }

        storageElements += (shape[i] - 1) * strides[i];
    }
    return storageElements;
}

bool IsTensorContiguous(const MLTensorDesc& desc) noexcept
{
    if (desc.shape.empty())
    {
        return false;
    }

    if (desc.strides.empty())
    {
        return true;
    }

    if (desc.strides.size() != desc.shape.size())
    {
        return false;
    }

    const MLSizes defaultStrides = GetDefaultTensorStrides(desc.shape);
    for (size_t i = 0; i < desc.strides.size(); ++i)
    {
        if (desc.strides[i] != defaultStrides[i])
        {
            return false;
        }
    }

    return true;
}

size_t GetTensorStorageElementCount(const MLTensorDesc& desc)
{
    if (!ValidateTensorDesc(desc))
    {
        return 0;
    }

    if (desc.strides.empty())
    {
        return GetTensorElementCount(desc.shape);
    }

    return GetTensorStorageElementCount(desc.shape, desc.strides);
}

size_t GetTensorStorageSize(const MLTensorDesc& desc)
{
    return GetTensorStorageElementCount(desc) * GetDataTypeSize(desc.dataType);
}

bool ValidateTensorDesc(const MLTensorDesc& desc) noexcept
{
    if (desc.dataType == MLDataType::Undefined)
    {
        return false;
    }

    if (desc.shape.empty())
    {
        return false;
    }

    for (const size_t dim : desc.shape)
    {
        if (dim == 0)
        {
            return false;
        }
    }

    if (desc.strides.empty())
    {
        return true;
    }

    return desc.strides.size() == desc.shape.size();
}

} // namespace rad
