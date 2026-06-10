#include <rad/ML/ML.test.h>

#include <rad/ML/Cpu/CpuMLActivations.h>
#include <rad/ML/Cpu/CpuMLDevice.h>
#include <rad/ML/MLContext.h>
#include <rad/ML/MLDevice.h>
#include <rad/ML/MLGlobal.h>
#include <rad/ML/MLTensor.h>

#include <cmath>
#include <stdexcept>
#include <vector>

using namespace rad;

namespace
{

constexpr float kEps = 1e-5f;

void FillInputTensor(MLTensor* tensor, const std::vector<float>& values)
{
    float* data = static_cast<float*>(tensor->MapMemory());
    for (size_t i = 0; i < values.size(); ++i)
    {
        data[i] = values[i];
    }
    tensor->UnmapMemory();
}

std::vector<float> ReadOutputTensor(MLTensor* tensor)
{
    const size_t elementCount = tensor->GetElementCount();
    std::vector<float> values(elementCount);
    const float* data = static_cast<const float*>(tensor->MapMemory());
    for (size_t i = 0; i < elementCount; ++i)
    {
        values[i] = data[i];
    }
    tensor->UnmapMemory();
    return values;
}

Ref<CpuMLDevice> GetCpuDevice(MLTensor* tensor)
{
    return Ref<CpuMLDevice>(static_cast<CpuMLDevice*>(tensor->GetDevice()));
}

void ExecuteUnaryOp(Ref<MLOp> op, MLTensor* input, MLTensor* output)
{
    MLDevice* device = op->GetDevice();
    ASSERT_NE(device, nullptr);
    Ref<MLContext> context = device->CreateContext();
    context->Execute(op.get(), input, output);
    context->Synchronize();
}

void ExpectFloatNear(const std::vector<float>& actual, const std::vector<float>& expected,
                     float tolerance = kEps)
{
    ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); ++i)
    {
        EXPECT_NEAR(actual[i], expected[i], tolerance) << "index " << i;
    }
}

} // namespace

TEST(CpuML, MLOpRelu)
{
    Ref<MLTensor> input = MLCreateTensor({5}, MLDataType::Float32);
    Ref<MLTensor> output = MLCreateTensor({5}, MLDataType::Float32);
    FillInputTensor(input.get(), {-2.0f, -0.5f, 0.0f, 1.0f, 3.5f});

    Ref<MLOp> op = CpuMLCreateReluOp(GetCpuDevice(input.get()));
    EXPECT_STREQ(op->GetName(), "Relu");

    {
        ExecuteUnaryOp(op, input.get(), output.get());
        ExpectFloatNear(ReadOutputTensor(output.get()), {0.0f, 0.0f, 0.0f, 1.0f, 3.5f});
    }

    {
        Ref<MLContext> context = input->GetDevice()->CreateContext();
        context->Relu(input.get(), output.get());
        context->Synchronize();
        ExpectFloatNear(ReadOutputTensor(output.get()), {0.0f, 0.0f, 0.0f, 1.0f, 3.5f});
    }

    {
        Ref<MLTensor> tensor = MLCreateTensor({3}, MLDataType::Float32);
        FillInputTensor(tensor.get(), {-1.0f, 0.0f, 2.0f});
        ExecuteUnaryOp(op, tensor.get(), tensor.get());
        ExpectFloatNear(ReadOutputTensor(tensor.get()), {0.0f, 0.0f, 2.0f});
    }

    {
        Ref<MLTensor> input = MLCreateTensor({3}, MLDataType::Float32);
        Ref<MLTensor> output = MLCreateTensor({3}, MLDataType::Float32);
        FillInputTensor(input.get(), {-1.0f, 0.0f, 2.0f});
        ExecuteUnaryOp(op, input.get(), output.get());
        ExpectFloatNear(ReadOutputTensor(output.get()), {0.0f, 0.0f, 2.0f});
    }

    {
        Ref<MLTensor> input = MLCreateTensor({5}, MLDataType::Float32);
        Ref<MLTensor> output = MLCreateTensor({5}, MLDataType::Float32);
        FillInputTensor(input.get(), {-2.0f, -0.5f, 0.0f, 1.0f, 3.5f});
        ExecuteUnaryOp(op, input.get(), output.get());
        ExpectFloatNear(ReadOutputTensor(output.get()), {0.0f, 0.0f, 0.0f, 1.0f, 3.5f});
    }

    {
        MLTensorDesc inputDesc;
        inputDesc.shape = {2, 3};
        inputDesc.strides = {1, 2};
        inputDesc.dataType = MLDataType::Float32;

        MLTensorDesc outputDesc;
        outputDesc.shape = {2, 3};
        outputDesc.strides = {3, 1};
        outputDesc.dataType = MLDataType::Float32;

        Ref<MLTensor> input = MLCreateTensor(inputDesc);
        Ref<MLTensor> output = MLCreateTensor(outputDesc);

        float* inputData = static_cast<float*>(input->MapMemory());
        inputData[0] = -2.0f;
        inputData[1] = -1.0f;
        inputData[2] = 0.0f;
        inputData[3] = 1.0f;
        inputData[4] = 2.0f;
        inputData[5] = 3.0f;
        input->UnmapMemory();

        ExecuteUnaryOp(op, input.get(), output.get());

        const std::vector<float> result = ReadOutputTensor(output.get());
        ASSERT_EQ(result.size(), 6u);
        EXPECT_FLOAT_EQ(result[0], 0.0f);
        EXPECT_FLOAT_EQ(result[1], 0.0f);
        EXPECT_FLOAT_EQ(result[2], 2.0f);
        EXPECT_FLOAT_EQ(result[3], 0.0f);
        EXPECT_FLOAT_EQ(result[4], 1.0f);
        EXPECT_FLOAT_EQ(result[5], 3.0f);
    }

    {
        MLTensorDesc desc;
        desc.shape = {2, 3};
        desc.strides = {1, 2};
        desc.dataType = MLDataType::Float32;

        Ref<MLTensor> tensor = MLCreateTensor(desc);

        float* data = static_cast<float*>(tensor->MapMemory());
        data[0] = -2.0f;
        data[1] = -1.0f;
        data[2] = 0.0f;
        data[3] = 1.0f;
        data[4] = 2.0f;
        data[5] = 3.0f;
        tensor->UnmapMemory();

        ExecuteUnaryOp(op, tensor.get(), tensor.get());

        const std::vector<float> result = ReadOutputTensor(tensor.get());
        ASSERT_EQ(result.size(), 6u);
        EXPECT_FLOAT_EQ(result[0], 0.0f);
        EXPECT_FLOAT_EQ(result[1], 0.0f);
        EXPECT_FLOAT_EQ(result[2], 0.0f);
        EXPECT_FLOAT_EQ(result[3], 1.0f);
        EXPECT_FLOAT_EQ(result[4], 2.0f);
        EXPECT_FLOAT_EQ(result[5], 3.0f);
    }
}


TEST(CpuML, MLOpLeakyRelu)
{
    Ref<MLTensor> input = MLCreateTensor({4}, MLDataType::Float32);
    Ref<MLTensor> output = MLCreateTensor({4}, MLDataType::Float32);
    FillInputTensor(input.get(), {-2.0f, -1.0f, 0.0f, 2.0f});

    Ref<MLOp> op = CpuMLCreateLeakyReluOp(GetCpuDevice(input.get()), 0.1f);
    EXPECT_STREQ(op->GetName(), "LeakyRelu");
    ExecuteUnaryOp(op, input.get(), output.get());
    ExpectFloatNear(ReadOutputTensor(output.get()), {-0.2f, -0.1f, 0.0f, 2.0f});

    Ref<MLTensor> contextInput = MLCreateTensor({2}, MLDataType::Float32);
    Ref<MLTensor> contextOutput = MLCreateTensor({2}, MLDataType::Float32);
    FillInputTensor(contextInput.get(), {-2.0f, 2.0f});

    Ref<MLContext> context = contextInput->GetDevice()->CreateContext();
    context->LeakyRelu(contextInput.get(), contextOutput.get(), 0.1f);
    context->Synchronize();
    ExpectFloatNear(ReadOutputTensor(contextOutput.get()), {-0.2f, 2.0f});

    context->LeakyRelu(contextInput.get(), contextOutput.get(), 0.25f);
    context->Synchronize();
    ExpectFloatNear(ReadOutputTensor(contextOutput.get()), {-0.5f, 2.0f});
}

TEST(CpuML, MLOpRejectsOverlappingOutputView)
{
    Ref<MLTensor> input = MLCreateTensor({2}, MLDataType::Float32);
    FillInputTensor(input.get(), {-2.0f, 2.0f});

    Ref<MLOp> op = CpuMLCreateLeakyReluOp(GetCpuDevice(input.get()), 0.1f);

    {
        MLTensorDesc outputDesc;
        outputDesc.shape = {2};
        outputDesc.strides = {0};
        outputDesc.dataType = MLDataType::Float32;

        Ref<MLTensor> output = MLCreateTensor(outputDesc);
        EXPECT_THROW(ExecuteUnaryOp(op, input.get(), output.get()), std::invalid_argument);
    }

    {
        MLTensorDesc inputDesc;
        inputDesc.shape = {2, 2};
        inputDesc.dataType = MLDataType::Float32;
        Ref<MLTensor> input2d = MLCreateTensor(inputDesc);
        FillInputTensor(input2d.get(), {-2.0f, -1.0f, 1.0f, 2.0f});

        MLTensorDesc outputDesc;
        outputDesc.shape = {2, 2};
        outputDesc.strides = {1, 1};
        outputDesc.dataType = MLDataType::Float32;

        Ref<MLTensor> output = MLCreateTensor(outputDesc);
        EXPECT_THROW(ExecuteUnaryOp(op, input2d.get(), output.get()), std::invalid_argument);
    }
}

TEST(CpuML, MLOpSigmoid)
{
    Ref<MLTensor> input = MLCreateTensor({3}, MLDataType::Float32);
    Ref<MLTensor> output = MLCreateTensor({3}, MLDataType::Float32);
    FillInputTensor(input.get(), {-2.0f, 0.0f, 2.0f});

    Ref<MLOp> op = CpuMLCreateSigmoidOp(GetCpuDevice(input.get()));
    EXPECT_STREQ(op->GetName(), "Sigmoid");
    ExecuteUnaryOp(op, input.get(), output.get());

    const float sig0 = 1.0f / (1.0f + std::exp(2.0f));
    const float sig2 = 1.0f / (1.0f + std::exp(-2.0f));
    ExpectFloatNear(ReadOutputTensor(output.get()), {sig0, 0.5f, sig2});
}

TEST(CpuML, MLOpTanh)
{
    Ref<MLTensor> input = MLCreateTensor({3}, MLDataType::Float32);
    Ref<MLTensor> output = MLCreateTensor({3}, MLDataType::Float32);
    FillInputTensor(input.get(), {-1.0f, 0.0f, 1.0f});

    Ref<MLOp> op = CpuMLCreateTanhOp(GetCpuDevice(input.get()));
    EXPECT_STREQ(op->GetName(), "Tanh");
    ExecuteUnaryOp(op, input.get(), output.get());

    ExpectFloatNear(ReadOutputTensor(output.get()), {std::tanh(-1.0f), 0.0f, std::tanh(1.0f)});
}

TEST(CpuML, MLOpGelu)
{
    Ref<MLTensor> input = MLCreateTensor({2}, MLDataType::Float32);
    Ref<MLTensor> output = MLCreateTensor({2}, MLDataType::Float32);
    FillInputTensor(input.get(), {-1.0f, 1.0f});

    {
        Ref<MLOp> op = CpuMLCreateGeluOp(GetCpuDevice(input.get()));
        EXPECT_STREQ(op->GetName(), "Gelu");
        ExecuteUnaryOp(op, input.get(), output.get());

        const auto gelu = [](float x) { return 0.5f * x * (1.0f + std::erf(x / std::sqrt(2.0f))); };
        ExpectFloatNear(ReadOutputTensor(output.get()), {gelu(-1.0f), gelu(1.0f)}, 1e-4f);
    }

    {
        Ref<MLOp> op = CpuMLCreateFastGeluOp(GetCpuDevice(input.get()));
        EXPECT_STREQ(op->GetName(), "FastGelu");
        FillInputTensor(input.get(), {-1.0f, 1.0f});
        ExecuteUnaryOp(op, input.get(), output.get());

        const auto fastGelu = [](float x) {
            const float inner = std::sqrt(2.0f / 3.14159265358979323846f) * (x + 0.044715f * x * x * x);
            return 0.5f * x * (1.0f + std::tanh(inner));
        };
        ExpectFloatNear(ReadOutputTensor(output.get()), {fastGelu(-1.0f), fastGelu(1.0f)}, 1e-4f);
    }
}

TEST(CpuML, MLOpSilu)
{
    Ref<MLTensor> input = MLCreateTensor({2}, MLDataType::Float32);
    Ref<MLTensor> output = MLCreateTensor({2}, MLDataType::Float32);
    FillInputTensor(input.get(), {-1.0f, 1.0f});

    Ref<MLOp> op = CpuMLCreateSiluOp(GetCpuDevice(input.get()));
    EXPECT_STREQ(op->GetName(), "Silu");
    ExecuteUnaryOp(op, input.get(), output.get());

    const auto silu = [](float x)
    {
        const float sigmoid = 1.0f / (1.0f + std::exp(-x));
        return x * sigmoid;
    };
    ExpectFloatNear(ReadOutputTensor(output.get()), {silu(-1.0f), silu(1.0f)});
}

TEST(CpuML, MLOpSoftplus)
{
    Ref<MLTensor> input = MLCreateTensor({3}, MLDataType::Float32);
    Ref<MLTensor> output = MLCreateTensor({3}, MLDataType::Float32);
    FillInputTensor(input.get(), {-2.0f, 0.0f, 3.0f});

    Ref<MLOp> op = CpuMLCreateSoftplusOp(GetCpuDevice(input.get()));
    EXPECT_STREQ(op->GetName(), "Softplus");
    ExecuteUnaryOp(op, input.get(), output.get());

    const auto softplus = [](float x) { return std::log1p(std::exp(x)); };
    ExpectFloatNear(ReadOutputTensor(output.get()),
                    {softplus(-2.0f), softplus(0.0f), softplus(3.0f)});
}

TEST(CpuML, MLOpElu)
{
    Ref<MLTensor> input = MLCreateTensor({3}, MLDataType::Float32);
    Ref<MLTensor> output = MLCreateTensor({3}, MLDataType::Float32);
    FillInputTensor(input.get(), {-1.0f, 0.0f, 1.0f});

    Ref<MLOp> op = CpuMLCreateEluOp(GetCpuDevice(input.get()), 2.0f);
    EXPECT_STREQ(op->GetName(), "Elu");
    ExecuteUnaryOp(op, input.get(), output.get());

    const auto elu = [](float x) { return x > 0.0f ? x : 2.0f * (std::exp(x) - 1.0f); };
    ExpectFloatNear(ReadOutputTensor(output.get()), {elu(-1.0f), elu(0.0f), elu(1.0f)});
}
