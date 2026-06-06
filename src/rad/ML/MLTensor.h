#pragma once

#include <rad/ML/MLCommon.h>
#include <rad/ML/MLBuffer.h>

#include <rad/Common/RefCounted.h>
#include <rad/Common/Span.h>

#include <cstddef>
#include <string_view>

namespace rad
{

class MLDevice;

// Tensor view over device buffer storage (see MLTensorDesc).
class MLTensor : public RefCounted<MLTensor>
{
public:
    MLTensor(const MLTensor&) = delete;
    MLTensor& operator=(const MLTensor&) = delete;

    [[nodiscard]] virtual MLDevice* GetDevice() const noexcept = 0;
    [[nodiscard]] virtual const MLTensorDesc& GetDesc() const noexcept = 0;

    [[nodiscard]] virtual MLBuffer* GetBuffer() const noexcept = 0;
    [[nodiscard]] virtual size_t GetBufferOffset() const noexcept = 0;

    [[nodiscard]] virtual size_t GetElementCount() const;
    [[nodiscard]] virtual size_t GetBufferSize() const; // Byte size of this view in the buffer.
    [[nodiscard]] virtual bool IsContiguous() const noexcept;

    // Maps this tensor's byte range in its buffer.
    [[nodiscard]] virtual void* MapMemory();
    virtual void UnmapMemory() noexcept;

    virtual ~MLTensor();

protected:
    MLTensor() = default;
}; // class MLTensor

// Allocates a new buffer on deviceId.
[[nodiscard]] Ref<MLTensor> CreateMLTensor(const MLTensorDesc& desc,
                                           std::string_view deviceId = "cpu:0");

// View into an existing buffer; device is taken from the buffer.
[[nodiscard]] Ref<MLTensor> CreateMLTensor(const MLTensorDesc& desc, Ref<MLBuffer> buffer,
                                           size_t bufferOffset);

[[nodiscard]] Ref<MLTensor> CreateMLTensor(Span<const size_t> shape, MLDataType dataType,
                                           std::string_view deviceId = "cpu:0");

} // namespace rad
