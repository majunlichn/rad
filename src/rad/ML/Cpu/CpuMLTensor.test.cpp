#include <rad/ML/ML.test.h>

#include <rad/ML/MLBuffer.h>
#include <rad/ML/MLCommon.h>
#include <rad/ML/MLDevice.h>
#include <rad/ML/MLGlobal.h>
#include <rad/ML/MLTensor.h>

using namespace rad;

TEST(ML, TensorDescTest)
{
    MLTensorDesc desc;
    desc.shape = {3, 4};
    desc.strides = {0, 1};
    desc.dataType = MLDataType::Float32;

    EXPECT_TRUE(ValidateTensorDesc(desc));
    EXPECT_FALSE(IsTensorContiguous(desc));
    EXPECT_EQ(GetTensorElementCount(desc.shape), 12u);
    EXPECT_EQ(GetTensorStorageElementCount(desc.shape, desc.strides), 4u);
}

TEST(ML, CpuMLTensorTest)
{
    Ref<MLTensor> tensor = CreateMLTensor({512, 512}, MLDataType::Float32);
    ASSERT_NE(tensor, nullptr);
    EXPECT_EQ(tensor->GetElementCount(), 512 * 512);
    EXPECT_EQ(tensor->GetBufferSize(), 512 * 512 * 4);
    EXPECT_EQ(tensor->GetDesc().shape.size(), 2);
    EXPECT_EQ(tensor->GetDesc().shape[0], 512);
    EXPECT_EQ(tensor->GetDesc().shape[1], 512);
    EXPECT_EQ(tensor->GetBufferOffset(), 0);
    EXPECT_TRUE(tensor->IsContiguous());
    EXPECT_TRUE(tensor->GetDesc().strides.empty());

    const MLSizes strides = GetTensorStrides(tensor->GetDesc());
    ASSERT_EQ(strides.size(), 2);
    EXPECT_EQ(strides[0], 512);
    EXPECT_EQ(strides[1], 1);

    MLBuffer* buffer = tensor->GetBuffer();
    ASSERT_NE(buffer, nullptr);
    EXPECT_EQ(buffer->GetSize(), 512 * 512 * 4);

    void* mapped = tensor->MapMemory();
    ASSERT_NE(mapped, nullptr);
    tensor->UnmapMemory();
}
