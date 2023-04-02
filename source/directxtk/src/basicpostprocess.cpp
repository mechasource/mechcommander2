//--------------------------------------------------------------------------------------
// File: BasicPostProcess.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include "d3dx12.h"
#include "postprocess.h"
#include "alignednew.h"
#include "commonstates.h"
#include "demandcreate.h"
#include "directxhelpers.h"
#include "effectpipelinestatedescription.h"
#include "graphicsmemory.h"
#include "sharedresourcepool.h"

using namespace directxtk;

// using Microsoft::WRL::ComPtr;

namespace
{
    constexpr int32_t c_MaxSamples = 16;

    constexpr int32_t Dirty_ConstantBuffer  = 0x01;
    constexpr int32_t Dirty_Parameters      = 0x02;

    constexpr int32_t RootSignatureCount = 2;

    // Constant buffer layout. Must match the shader!
    __declspec(align(16)) struct PostProcessConstants
    {
        DirectX::XMVECTOR sampleOffsets[c_MaxSamples];
        DirectX::XMVECTOR sampleWeights[c_MaxSamples];
    };

    static_assert((sizeof(PostProcessConstants) % 16) == 0, "CB size not padded correctly");

    // 2-parameter Gaussian distribution given standard deviation (rho)
    inline float GaussianDistribution(float x, float y, float rho) noexcept
    {
        return expf(-(x * x + y * y) / (2 * rho * rho)) / sqrtf(2 * DirectX::XM_PI * rho * rho);
    }
}

// Include the precompiled shader code.
namespace
{
#if defined(_XBOX_ONE) && defined(_TITLE)
    #include "Shaders/Compiled/XboxOnePostProcess_VSQuadNoCB.inc"
    #include "Shaders/Compiled/XboxOnePostProcess_VSQuad.inc"

    #include "Shaders/Compiled/XboxOnePostProcess_PSCopy.inc"
    #include "Shaders/Compiled/XboxOnePostProcess_PSMonochrome.inc"
    #include "Shaders/Compiled/XboxOnePostProcess_PSSepia.inc"
    #include "Shaders/Compiled/XboxOnePostProcess_PSDownScale2x2.inc"
    #include "Shaders/Compiled/XboxOnePostProcess_PSDownScale4x4.inc"
    #include "Shaders/Compiled/XboxOnePostProcess_PSGaussianBlur5x5.inc"
    #include "Shaders/Compiled/XboxOnePostProcess_PSBloomExtract.inc"
    #include "Shaders/Compiled/XboxOnePostProcess_PSBloomBlur.inc"
#else
    #include "Shaders/Compiled/PostProcess_VSQuadNoCB.inc"
    #include "Shaders/Compiled/PostProcess_VSQuad.inc"

    #include "Shaders/Compiled/PostProcess_PSCopy.inc"
    #include "Shaders/Compiled/PostProcess_PSMonochrome.inc"
    #include "Shaders/Compiled/PostProcess_PSSepia.inc"
    #include "Shaders/Compiled/PostProcess_PSDownScale2x2.inc"
    #include "Shaders/Compiled/PostProcess_PSDownScale4x4.inc"
    #include "Shaders/Compiled/PostProcess_PSGaussianBlur5x5.inc"
    #include "Shaders/Compiled/PostProcess_PSBloomExtract.inc"
    #include "Shaders/Compiled/PostProcess_PSBloomBlur.inc"
#endif
}

namespace
{
    const D3D12_SHADER_BYTECODE vertexShader[] =
    {
        { PostProcess_VSQuadNoCB,               sizeof(PostProcess_VSQuadNoCB) },
        { PostProcess_VSQuad,                   sizeof(PostProcess_VSQuad) },
    };

    const D3D12_SHADER_BYTECODE pixelShaders[] =
    {
        { PostProcess_PSCopy,                   sizeof(PostProcess_PSCopy) },
        { PostProcess_PSMonochrome,             sizeof(PostProcess_PSMonochrome) },
        { PostProcess_PSSepia,                  sizeof(PostProcess_PSSepia) },
        { PostProcess_PSDownScale2x2,           sizeof(PostProcess_PSDownScale2x2) },
        { PostProcess_PSDownScale4x4,           sizeof(PostProcess_PSDownScale4x4) },
        { PostProcess_PSGaussianBlur5x5,        sizeof(PostProcess_PSGaussianBlur5x5) },
        { PostProcess_PSBloomExtract,           sizeof(PostProcess_PSBloomExtract) },
        { PostProcess_PSBloomBlur,              sizeof(PostProcess_PSBloomBlur) },
    };

    static_assert(_countof(pixelShaders) == BasicPostProcess::Effect_Max, "array/max mismatch");

    // Factory for lazily instantiating shared root signatures.
    class DeviceResources
    {
    public:
        DeviceResources(_In_ ID3D12Device* device) noexcept
            : m_device(device),
            m_prootsignature{},
            mMutex{}
        { }

        ID3D12RootSignature* GetRootSignature(int32_t slot, const D3D12_ROOT_SIGNATURE_DESC& desc)
        {
            assert(slot >= 0 && slot < RootSignatureCount);
            _Analysis_assume_(slot >= 0 && slot < RootSignatureCount);

            return DemandCreate(m_prootsignature[slot], mMutex, [&](ID3D12RootSignature** pResult) noexcept -> HRESULT
            {
                HRESULT hr = CreateRootSignature(m_device.get(), &desc, pResult);

                if (SUCCEEDED(hr))
                    SetDebugObjectName(*pResult, L"BasicPostProcess");

                return hr;
            });
        }

        ID3D12Device* GetDevice() const noexcept { return m_device.get(); }

    protected:
        wil::com_ptr<ID3D12Device>                        m_device;
        wil::com_ptr<ID3D12RootSignature> m_prootsignature[RootSignatureCount];
        std::mutex                                  mMutex;
    };
}

class BasicPostProcess::Impl : public AlignedNew<PostProcessConstants>
{
public:
    Impl(_In_ ID3D12Device* device, const RenderTargetState& rtState, Effect ifx);

    void Process(_In_ ID3D12GraphicsCommandList* commandList);

    void SetDirtyFlag() noexcept { mDirtyFlags = INT_MAX; }

    enum RootParameterIndex
    {
        TextureSRV,
        ConstantBuffer,
        RootParameterCount
    };

    // Fields.
    BasicPostProcess::Effect                fx;
    PostProcessConstants                    constants;
    D3D12_GPU_DESCRIPTOR_HANDLE             texture;
    uint32_t                                texWidth;
    uint32_t                                texHeight;
    float                                   guassianMultiplier;
    float                                   bloomSize;
    float                                   bloomBrightness;
    float                                   bloomThreshold;
    bool                                    bloomHorizontal;

private:
    bool                                    mUseConstants;
    int32_t                                     mDirtyFlags;

    void                                    DownScale2x2();
    void                                    DownScale4x4();
    void                                    GaussianBlur5x5(float multiplier);
    void                                    Bloom(bool horizontal, float size, float brightness);

    // D3D constant buffer holds a copy of the same data as the public 'constants' field.
    GraphicsResource mConstantBuffer;

    // Per instance cache of PSOs, populated with variants for each shader & layout
    wil::com_ptr<ID3D12PipelineState> m_ppipelinestate;

    // Per instance root signature
    ID3D12RootSignature* m_prootsignature;

    // Per-device resources.
    std::shared_ptr<DeviceResources> mDeviceResources;

    static SharedResourcePool<ID3D12Device*, DeviceResources> deviceResourcesPool;
};


// Global pool of per-device BasicPostProcess resources.
SharedResourcePool<ID3D12Device*, DeviceResources> BasicPostProcess::Impl::deviceResourcesPool;


// Constructor.
BasicPostProcess::Impl::Impl(_In_ ID3D12Device* device, const RenderTargetState& rtState, Effect ifx)
    : fx(ifx),
    constants{},
    texture{},
    texWidth(0),
    texHeight(0),
    guassianMultiplier(1.f),
    bloomSize(1.f),
    bloomBrightness(1.f),
    bloomThreshold(0.25f),
    bloomHorizontal(true),
    mDirtyFlags(INT_MAX),
    mDeviceResources(deviceResourcesPool.DemandCreate(device))
{
    if (ifx >= Effect_Max)
        throw std::out_of_range("Effect not defined");
   
    switch (ifx)
    {
    case Copy:
    case Monochrome:
    case Sepia:
        // These shaders don't use the constant buffer
        mUseConstants = false;
        break;

    default:
        mUseConstants = true;
        break;
    }

    // Create root signature.
    {
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

        CD3DX12_DESCRIPTOR_RANGE textureSRVs(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        // Same as CommonStates::StaticLinearClamp
        CD3DX12_STATIC_SAMPLER_DESC sampler(
            0, // register
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            0.f,
            16,
            D3D12_COMPARISON_FUNC_LESS_EQUAL,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
            0.f,
            D3D12_FLOAT32_MAX,
            D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};
        rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSRVs, D3D12_SHADER_VISIBILITY_PIXEL);

        // Root parameter descriptor - conditionally initialized
        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc = {};

        if (mUseConstants)
        {
            // Include constant buffer
            rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

            // use all parameters
            rsigDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

            m_prootsignature = mDeviceResources->GetRootSignature(1, rsigDesc);
        }
        else
        {
            // only use constant
            rsigDesc.Init(1, rootParameters, 1, &sampler, rootSignatureFlags);

            m_prootsignature = mDeviceResources->GetRootSignature(0, rsigDesc);
        }
    }

    assert(m_prootsignature != nullptr);

    // Create pipeline state.
    EffectPipelineStateDescription psd(nullptr,
        CommonStates::Opaque,
        CommonStates::DepthNone,
        CommonStates::CullNone,
        rtState,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

    psd.CreatePipelineState(
        device,
        m_prootsignature,
        vertexShader[mUseConstants ? 1 : 0],
        pixelShaders[ifx],
        m_ppipelinestate.addressof());

    SetDebugObjectName(m_ppipelinestate.get(), L"BasicPostProcess");
}


// Sets our state onto the D3D device.
void BasicPostProcess::Impl::Process(_In_ ID3D12GraphicsCommandList* commandList)
{
    // Set the root signature.
    commandList->SetGraphicsRootSignature(m_prootsignature);

    // Set the texture.
    if (!texture.ptr)
    {
        DebugTrace("ERROR: Missing texture for BasicPostProcess (texture %llu)\n", texture.ptr);
        throw std::exception("BasicPostProcess");
    }
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);

    // Set constants.
    if (mUseConstants)
    {
        if (mDirtyFlags & Dirty_Parameters)
        {
            mDirtyFlags &= ~Dirty_Parameters;
            mDirtyFlags |= Dirty_ConstantBuffer;

            switch (fx)
            {
            case DownScale_2x2:
                DownScale2x2();
                break;

            case DownScale_4x4:
                DownScale4x4();
                break;

            case GaussianBlur_5x5:
                GaussianBlur5x5(guassianMultiplier);
                break;

            case BloomExtract:
                constants.sampleWeights[0] = DirectX::XMVectorReplicate(bloomThreshold);
                break;

            case BloomBlur:
                Bloom(bloomHorizontal, bloomSize, bloomBrightness);
                break;

            default:
                break;
            }
        }

        if (mDirtyFlags & Dirty_ConstantBuffer)
        {
            mDirtyFlags &= ~Dirty_ConstantBuffer;
            mConstantBuffer = GraphicsMemory::Get(mDeviceResources->GetDevice()).AllocateConstant(constants);
        }

        commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, mConstantBuffer.GpuAddress());
    }

    // Set the pipeline state.
    commandList->SetPipelineState(m_ppipelinestate.get());

    // Draw quad.
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawInstanced(3, 1, 0, 0);
}


void BasicPostProcess::Impl::DownScale2x2()
{
    mUseConstants = true;

    if (!texWidth || !texHeight)
    {
        throw std::exception("Call SetSourceTexture before setting post-process effect");
    }

    float tu = 1.0f / float(texWidth);
    float tv = 1.0f / float(texHeight);

    // Sample from the 4 surrounding points. Since the center point will be in the exact
    // center of 4 texels, a 0.5f offset is needed to specify a texel center.
    auto ptr = reinterpret_cast<DirectX::XMFLOAT4*>(constants.sampleOffsets);
    for (int32_t y = 0; y < 2; ++y)
    {
        for (int32_t x = 0; x < 2; ++x)
        {
            ptr->x = (float(x) - 0.5f) * tu;
            ptr->y = (float(y) - 0.5f) * tv;
            ++ptr;
        }
    }
}


void BasicPostProcess::Impl::DownScale4x4()
{
    mUseConstants = true;

    if (!texWidth || !texHeight)
    {
        throw std::exception("Call SetSourceTexture before setting post-process effect");
    }

    float tu = 1.0f / float(texWidth);
    float tv = 1.0f / float(texHeight);

    // Sample from the 16 surrounding points. Since the center point will be in the
    // exact center of 16 texels, a 1.5f offset is needed to specify a texel center.
    auto ptr = reinterpret_cast<DirectX::XMFLOAT4*>(constants.sampleOffsets);
    for (int32_t y = 0; y < 4; ++y)
    {
        for (int32_t x = 0; x < 4; ++x)
        {
            ptr->x = (float(x) - 1.5f) * tu;
            ptr->y = (float(y) - 1.5f) * tv;
            ++ptr;
        }
    }

}


void BasicPostProcess::Impl::GaussianBlur5x5(float multiplier)
{
    mUseConstants = true;

    if (!texWidth || !texHeight)
    {
        throw std::exception("Call SetSourceTexture before setting post-process effect");
    }

    float tu = 1.0f / float(texWidth);
    float tv = 1.0f / float(texHeight);

    float totalWeight = 0.0f;
    size_t index = 0;
    auto offsets = reinterpret_cast<DirectX::XMFLOAT4*>(constants.sampleOffsets);
    auto weights = constants.sampleWeights;
    for (int32_t x = -2; x <= 2; ++x)
    {
        for (int32_t y = -2; y <= 2; ++y)
        {
            // Exclude pixels with a block distance greater than 2. This will
            // create a kernel which approximates a 5x5 kernel using only 13
            // sample points instead of 25; this is necessary since 2.0 shaders
            // only support 16 texture grabs.
            if (fabsf(float(x)) + fabsf(float(y)) > 2.0f)
                continue;

            // Get the unscaled Gaussian intensity for this offset
            offsets[index].x = float(x) * tu;
            offsets[index].y = float(y) * tv;
            offsets[index].z = 0.0f;
            offsets[index].w = 0.0f;

            float g = GaussianDistribution(float(x), float(y), 1.0f);
            weights[index] = DirectX::XMVectorReplicate(g);

            totalWeight += DirectX::XMVectorGetX(weights[index]);

            ++index;
        }
    }

    // Divide the current weight by the total weight of all the samples; Gaussian
    // blur kernels add to 1.0f to ensure that the intensity of the image isn't
    // changed when the blur occurs. An optional multiplier variable is used to
    // add or remove image intensity during the blur.
    DirectX::XMVECTOR vtw = DirectX::XMVectorReplicate(totalWeight);
    DirectX::XMVECTOR vm = DirectX::XMVectorReplicate(multiplier);
    for (auto i = 0u; i < index; ++i)
    {
        DirectX::XMVECTOR w = DirectX::XMVectorDivide(weights[i], vtw);
        weights[i] = DirectX::XMVectorMultiply(w, vm);
    }
}


void  BasicPostProcess::Impl::Bloom(bool horizontal, float size, float brightness)
{
    mUseConstants = true;

    if (!texWidth || !texHeight)
    {
        throw std::exception("Call SetSourceTexture before setting post-process effect");
    }

    float tu = 0.f;
    float tv = 0.f;
    if (horizontal)
    {
        tu = 1.f / float(texWidth);
    }
    else
    {
        tv = 1.f / float(texHeight);
    }

    auto weights = reinterpret_cast<DirectX::XMFLOAT4*>(constants.sampleWeights);
    auto offsets = reinterpret_cast<DirectX::XMFLOAT4*>(constants.sampleOffsets);

    // Fill the center texel
    float weight = brightness * GaussianDistribution(0, 0, size);
    weights[0] = DirectX::XMFLOAT4(weight, weight, weight, 1.0f);
    offsets[0].x = offsets[0].y = offsets[0].z = offsets[0].w = 0.f;

    // Fill the first half
    for (int32_t i = 1; i < 8; ++i)
    {
        // Get the Gaussian intensity for this offset
        weight = brightness * GaussianDistribution(float(i), 0, size);
        weights[i] = DirectX::XMFLOAT4(weight, weight, weight, 1.0f);
        offsets[i] = DirectX::XMFLOAT4(float(i) * tu, float(i) * tv, 0.f, 0.f);
    }

    // Mirror to the second half
    for (int32_t i = 8; i < 15; i++)
    {
        weights[i] = weights[i - 7];
        offsets[i] = DirectX::XMFLOAT4(-offsets[i - 7].x, -offsets[i - 7].y, 0.f, 0.f);
    }
}


// Public constructor.
BasicPostProcess::BasicPostProcess(_In_ ID3D12Device* device, const RenderTargetState& rtState, Effect fx)
  : pimpl(std::make_unique<Impl>(device, rtState, fx))
{
}


// Move constructor.
BasicPostProcess::BasicPostProcess(BasicPostProcess&& moveFrom) noexcept
  : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
BasicPostProcess& BasicPostProcess::operator= (BasicPostProcess&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
BasicPostProcess::~BasicPostProcess()
{
}


// IPostProcess methods.
void BasicPostProcess::Process(_In_ ID3D12GraphicsCommandList* commandList)
{
    pimpl->Process(commandList);
}


// Properties
void BasicPostProcess::SetSourceTexture(D3D12_GPU_DESCRIPTOR_HANDLE srvDescriptor, _In_opt_ ID3D12Resource* resource)
{
    pimpl->texture = srvDescriptor;

    if (resource)
    {
        const auto desc = resource->GetDesc();
        pimpl->texWidth = static_cast<uint32_t>(desc.Width);
        pimpl->texHeight = desc.Height;
    }
    else
    {
        pimpl->texWidth = pimpl->texHeight = 0;
    }
}


void BasicPostProcess::SetGaussianParameter(float multiplier)
{
    pimpl->guassianMultiplier = multiplier;
    pimpl->SetDirtyFlag();
}


void BasicPostProcess::SetBloomExtractParameter(float threshold)
{
    pimpl->bloomThreshold = threshold;
    pimpl->SetDirtyFlag();
}


void BasicPostProcess::SetBloomBlurParameters(bool horizontal, float size, float brightness)
{
    pimpl->bloomSize = size;
    pimpl->bloomBrightness = brightness;
    pimpl->bloomHorizontal = horizontal;
    pimpl->SetDirtyFlag();
}
