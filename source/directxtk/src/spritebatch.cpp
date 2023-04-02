//--------------------------------------------------------------------------------------
// File: SpriteBatch.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"

#include "d3dx12.h"
#include "SpriteBatch.h"
#include "commonstates.h"
#include "VertexTypes.h"
#include "SharedResourcePool.h"
#include "AlignedNew.h"
#include "resourceuploadbatch.h"
#include "graphicsmemory.h"
#include "directxhelpers.h"

using namespace directxtk;
// using Microsoft::WRL::ComPtr;

namespace
{
    // Include the precompiled shader code.
    #if defined(_XBOX_ONE) && defined(_TITLE)
    #include "Shaders/Compiled/XboxOneSpriteEffect_SpriteVertexShader.inc"
    #include "Shaders/Compiled/XboxOneSpriteEffect_SpritePixelShader.inc"
    #include "Shaders/Compiled/XboxOneSpriteEffect_SpriteVertexShaderHeap.inc"
    #include "Shaders/Compiled/XboxOneSpriteEffect_SpritePixelShaderHeap.inc"
    #else
    #include "Shaders/Compiled/SpriteEffect_SpriteVertexShader.inc"
    #include "Shaders/Compiled/SpriteEffect_SpritePixelShader.inc"
    #include "Shaders/Compiled/SpriteEffect_SpriteVertexShaderHeap.inc"
    #include "Shaders/Compiled/SpriteEffect_SpritePixelShaderHeap.inc"
    #endif

    inline bool operator != (D3D12_GPU_DESCRIPTOR_HANDLE a, D3D12_GPU_DESCRIPTOR_HANDLE b) noexcept
    {
        return a.ptr != b.ptr;
    }
    inline bool operator < (D3D12_GPU_DESCRIPTOR_HANDLE a, D3D12_GPU_DESCRIPTOR_HANDLE b) noexcept
    {
        return a.ptr < b.ptr;
    }

    // Helper converts a RECT to DirectX::XMVECTOR.
    inline DirectX::XMVECTOR LoadRect(_In_ RECT const* rect) noexcept
    {
        DirectX::XMVECTOR v = DirectX::XMLoadInt4(reinterpret_cast<uint32_t const*>(rect));

        v = DirectX::XMConvertVectorIntToFloat(v, 0);

        // Convert right/bottom to width/height.
        v = DirectX::XMVectorSubtract(v, DirectX::XMVectorPermute<0, 1, 4, 5>(DirectX::g_XMZero, v));

        return v;
    }
}

// Internal SpriteBatch implementation class.
__declspec(align(16)) class SpriteBatch::Impl : public AlignedNew<SpriteBatch::Impl>
{
public:
    Impl(_In_ ID3D12Device* device,
         ResourceUploadBatch& upload,
         const SpriteBatchPipelineStateDescription& psoDesc,
         const D3D12_VIEWPORT* viewport);

    void XM_CALLCONV Begin(
        _In_ ID3D12GraphicsCommandList* commandList,
        SpriteSortMode sortMode = SpriteSortMode_Deferred,
        DirectX::FXMMATRIX transformMatrix = MatrixIdentity);
    void End();

    void XM_CALLCONV Draw(
        D3D12_GPU_DESCRIPTOR_HANDLE texture,
        DirectX::XMUINT2 const& textureSize,
        DirectX::FXMVECTOR destination,
        _In_opt_ RECT const* sourceRectangle,
        DirectX::FXMVECTOR color,
        DirectX::FXMVECTOR originRotationDepth,
        uint32_t flags);

    // Info about a single sprite that is waiting to be drawn.
    __declspec(align(16)) struct SpriteInfo : public AlignedNew<SpriteInfo>
    {
        DirectX::XMFLOAT4A source;
        DirectX::XMFLOAT4A destination;
        DirectX::XMFLOAT4A color;
        DirectX::XMFLOAT4A originRotationDepth;
        D3D12_GPU_DESCRIPTOR_HANDLE texture;
        DirectX::XMVECTOR textureSize;
        uint32_t flags;

        // Combine values from the public SpriteEffects enum with these internal-only flags.
        static const uint32_t SourceInTexels = 4;
        static const uint32_t DestSizeInPixels = 8;

        static_assert((SpriteEffects_FlipBoth & (SourceInTexels | DestSizeInPixels)) == 0, "Flag bits must not overlap");
    };

    DXGI_MODE_ROTATION mRotation;

    bool mSetViewport;
    D3D12_VIEWPORT mViewPort;
    D3D12_GPU_DESCRIPTOR_HANDLE mSampler;

private:
    // Implementation helper methods.
    void GrowSpriteQueue();
    void PrepareForRendering();
    void FlushBatch();
    void SortSprites();
    void GrowSortedSprites();

    void RenderBatch(
        D3D12_GPU_DESCRIPTOR_HANDLE texture,
        DirectX::XMVECTOR textureSize,
        _In_reads_(count) SpriteInfo const* const* sprites,
        size_t count);

    static void XM_CALLCONV RenderSprite(_In_ SpriteInfo const* sprite,
        _Out_writes_(VerticesPerSprite) VertexPositionColorTexture* vertices,
        DirectX::FXMVECTOR textureSize,
        DirectX::FXMVECTOR inverseTextureSize) noexcept;

    DirectX::XMMATRIX GetViewportTransform(_In_ DXGI_MODE_ROTATION rotation);

    // Constants.
    static const size_t MaxBatchSize = 2048;
    static const size_t MinBatchSize = 128;
    static const size_t InitialQueueSize = 64;
    static const size_t VerticesPerSprite = 4;
    static const size_t IndicesPerSprite = 6;

    //
    // The following functions and members are used to create the default pipeline state objects.
    //
    static const D3D12_SHADER_BYTECODE s_DefaultVertexShaderByteCodeStatic;
    static const D3D12_SHADER_BYTECODE s_DefaultPixelShaderByteCodeStatic;
    static const D3D12_SHADER_BYTECODE s_DefaultVertexShaderByteCodeHeap;
    static const D3D12_SHADER_BYTECODE s_DefaultPixelShaderByteCodeHeap;
    static const D3D12_INPUT_LAYOUT_DESC s_DefaultInputLayoutDesc;


    // Queue of sprites waiting to be drawn.
    std::unique_ptr<SpriteInfo[]> mSpriteQueue;

    size_t mSpriteQueueCount;
    size_t mSpriteQueueArraySize;


    // To avoid needlessly copying around bulky SpriteInfo structures, we leave that
    // actual data alone and just sort this array of pointers instead. But we want contiguous
    // memory for cache efficiency, so these pointers are just shortcuts into the single
    // mSpriteQueue array, and we take care to keep them in order when sorting is disabled.
    std::vector<SpriteInfo const*> mSortedSprites;


    // Mode settings from the last Begin call.
    bool mInBeginEndPair;

    SpriteSortMode mSortMode;
    wil::com_ptr<ID3D12PipelineState> mPSO;
    wil::com_ptr<ID3D12RootSignature> m_prootsignature;
    DirectX::XMMATRIX mTransformMatrix;
    wil::com_ptr<ID3D12GraphicsCommandList> mCommandList;

    // Batched data
    GraphicsResource mVertexSegment;
    size_t mVertexPageSize;
    size_t mSpriteCount;
    GraphicsResource mConstantBuffer;

    enum RootParameterIndex
    {
        TextureSRV,
        ConstantBuffer,
        TextureSampler,
        RootParameterCount
    };

    // Only one of these helpers is allocated per D3D device, even if there are multiple SpriteBatch instances.
    struct DeviceResources
    {
        DeviceResources(_In_ ID3D12Device* device, ResourceUploadBatch& upload);

        wil::com_ptr<ID3D12Resource> indexBuffer;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        wil::com_ptr<ID3D12RootSignature> rootSignatureStatic;
        wil::com_ptr<ID3D12RootSignature> rootSignatureHeap; 
        ID3D12Device* m_device;

    private:
        void CreateIndexBuffer(_In_ ID3D12Device* device, ResourceUploadBatch& upload);
        void CreateRootSignatures(_In_ ID3D12Device* device);

        static std::vector<short> CreateIndexValues();
    };

    // Per-device data.
    std::shared_ptr<DeviceResources> mDeviceResources;
    static SharedResourcePool<ID3D12Device*, DeviceResources, ResourceUploadBatch&> deviceResourcesPool;
};


// Global pools of per-device and per-context SpriteBatch resources.
SharedResourcePool<ID3D12Device*, SpriteBatch::Impl::DeviceResources, ResourceUploadBatch&> SpriteBatch::Impl::deviceResourcesPool;


// Constants.
const DirectX::XMMATRIX SpriteBatch::MatrixIdentity = DirectX::XMMatrixIdentity();
const DirectX::XMFLOAT2 SpriteBatch::Float2Zero(0, 0);

const D3D12_SHADER_BYTECODE SpriteBatch::Impl::s_DefaultVertexShaderByteCodeStatic = {SpriteEffect_SpriteVertexShader, sizeof(SpriteEffect_SpriteVertexShader)};
const D3D12_SHADER_BYTECODE SpriteBatch::Impl::s_DefaultPixelShaderByteCodeStatic = {SpriteEffect_SpritePixelShader, sizeof(SpriteEffect_SpritePixelShader)};

const D3D12_SHADER_BYTECODE SpriteBatch::Impl::s_DefaultVertexShaderByteCodeHeap = { SpriteEffect_SpriteVertexShaderHeap, sizeof(SpriteEffect_SpriteVertexShaderHeap) };
const D3D12_SHADER_BYTECODE SpriteBatch::Impl::s_DefaultPixelShaderByteCodeHeap = { SpriteEffect_SpritePixelShaderHeap, sizeof(SpriteEffect_SpritePixelShaderHeap) };

const D3D12_INPUT_LAYOUT_DESC SpriteBatch::Impl::s_DefaultInputLayoutDesc = VertexPositionColorTexture::InputLayout;

// Matches CommonStates::AlphaBlend
const D3D12_BLEND_DESC SpriteBatchPipelineStateDescription::s_DefaultBlendDesc =
{
    FALSE, // AlphaToCoverageEnable
    FALSE, // IndependentBlendEnable
    { {
        TRUE, // BlendEnable
        FALSE, // LogicOpEnable
        D3D12_BLEND_ONE, // SrcBlend
        D3D12_BLEND_INV_SRC_ALPHA, // DestBlend
        D3D12_BLEND_OP_ADD, // BlendOp
        D3D12_BLEND_ONE, // SrcBlendAlpha
        D3D12_BLEND_INV_SRC_ALPHA, // DestBlendAlpha
        D3D12_BLEND_OP_ADD, // BlendOpAlpha
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL
    } }
};

// Same to CommonStates::CullCounterClockwise
const D3D12_RASTERIZER_DESC SpriteBatchPipelineStateDescription::s_DefaultRasterizerDesc =
{
    D3D12_FILL_MODE_SOLID,
    D3D12_CULL_MODE_BACK,
    FALSE, // FrontCounterClockwise
    D3D12_DEFAULT_DEPTH_BIAS,
    D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE, // DepthClipEnable
    TRUE, // MultisampleEnable
    FALSE, // AntialiasedLineEnable
    0, // ForcedSampleCount
    D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
};

// Same as CommonStates::DepthNone
const D3D12_DEPTH_STENCIL_DESC SpriteBatchPipelineStateDescription::s_DefaultDepthStencilDesc =
{
    FALSE, // DepthEnable
    D3D12_DEPTH_WRITE_MASK_ZERO,
    D3D12_COMPARISON_FUNC_LESS_EQUAL, // DepthFunc
    FALSE, // StencilEnable
    D3D12_DEFAULT_STENCIL_READ_MASK,
    D3D12_DEFAULT_STENCIL_WRITE_MASK,
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    }, // FrontFace
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    } // BackFace
};

// Per-device constructor.
SpriteBatch::Impl::DeviceResources::DeviceResources(_In_ ID3D12Device* device, ResourceUploadBatch& upload) :
    indexBufferView{},
    m_device(device)
{
    CreateIndexBuffer(device, upload);
    CreateRootSignatures(device);
}

// Creates the SpriteBatch index buffer.
void SpriteBatch::Impl::DeviceResources::CreateIndexBuffer(_In_ ID3D12Device* device, ResourceUploadBatch& upload)
{
    static_assert((MaxBatchSize * VerticesPerSprite) < USHRT_MAX, "MaxBatchSize too large for 16-bit indices");

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(short) * MaxBatchSize * IndicesPerSprite);

    // Create the constant buffer.
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_GRAPHICS_PPV_ARGS(indexBuffer.put())));

    SetDebugObjectName(indexBuffer.get(), L"SpriteBatch");

    auto indexValues = CreateIndexValues();

    D3D12_SUBRESOURCE_DATA indexDataDesc = {};
    indexDataDesc.pData = indexValues.data();
    indexDataDesc.RowPitch = static_cast<intptr_t>(bufferDesc.Width);
    indexDataDesc.SlicePitch = indexDataDesc.RowPitch;

    // Upload the resource
    upload.Upload(indexBuffer.get(), 0, &indexDataDesc, 1);
    upload.Transition(indexBuffer.get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
    SetDebugObjectName(indexBuffer.get(), L"DirectXTK:SpriteBatch Index Buffer");

    // Create the index buffer view
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    indexBufferView.SizeInBytes = static_cast<uint32_t>(bufferDesc.Width);
}

void SpriteBatch::Impl::DeviceResources::CreateRootSignatures(_In_ ID3D12Device* device)
{
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

    CD3DX12_DESCRIPTOR_RANGE textureSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    {
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

        CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount - 1] = {};
        rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc;
        rsigDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

        ThrowIfFailed(::CreateRootSignature(device, &rsigDesc, rootSignatureStatic.put()));

        SetDebugObjectName(rootSignatureStatic.get(), L"SpriteBatch");
    }

    {
        CD3DX12_DESCRIPTOR_RANGE textureSampler(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};
        rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[RootParameterIndex::TextureSampler].InitAsDescriptorTable(1, &textureSampler, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc;
        rsigDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

        ThrowIfFailed(::CreateRootSignature(device, &rsigDesc, rootSignatureHeap.put()));

        SetDebugObjectName(rootSignatureHeap.get(), L"SpriteBatch");
    }
}

// Helper for populating the SpriteBatch index buffer.
std::vector<short> SpriteBatch::Impl::DeviceResources::CreateIndexValues()
{
    std::vector<short> indices;

    indices.reserve(MaxBatchSize * IndicesPerSprite);

    for (auto j = 0u; j < MaxBatchSize * VerticesPerSprite; j += VerticesPerSprite)
    {
        short i = static_cast<short>(j);

        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);

        indices.push_back(i + 1);
        indices.push_back(i + 3);
        indices.push_back(i + 2);
    }

    return indices;
}

// Per-SpriteBatch constructor.
_Use_decl_annotations_
SpriteBatch::Impl::Impl(ID3D12Device* device, ResourceUploadBatch& upload, const SpriteBatchPipelineStateDescription& psoDesc, const D3D12_VIEWPORT* viewport)
    : mRotation(DXGI_MODE_ROTATION_IDENTITY),
    mSetViewport(false),
    mViewPort{},
    mSampler{},
    mSpriteQueueCount(0),
    mSpriteQueueArraySize(0),
    mInBeginEndPair(false),
    mSortMode(SpriteSortMode_Deferred),
    mTransformMatrix(MatrixIdentity),
    mVertexSegment{},
    mVertexPageSize(sizeof(VertexPositionColorTexture) * MaxBatchSize * VerticesPerSprite),
    mSpriteCount(0),
    mDeviceResources(deviceResourcesPool.DemandCreate(device, upload))
{
    if (viewport != nullptr)
    {
        mViewPort = *viewport;
        mSetViewport = true;
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dDesc = {};
    d3dDesc.InputLayout = s_DefaultInputLayoutDesc;
    d3dDesc.BlendState = psoDesc.blendDesc;
    d3dDesc.DepthStencilState = psoDesc.depthStencilDesc;
    d3dDesc.RasterizerState = psoDesc.rasterizerDesc;
    d3dDesc.DSVFormat = psoDesc.renderTargetState.dsvFormat;
    d3dDesc.NodeMask = psoDesc.renderTargetState.nodeMask;
    d3dDesc.NumRenderTargets = psoDesc.renderTargetState.numRenderTargets;
    memcpy_s(d3dDesc.RTVFormats, sizeof(d3dDesc.RTVFormats), psoDesc.renderTargetState.rtvFormats, sizeof(DXGI_FORMAT) * D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
    d3dDesc.SampleDesc = psoDesc.renderTargetState.sampleDesc;
    d3dDesc.SampleMask = psoDesc.renderTargetState.sampleMask;
    d3dDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    d3dDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // Three choices: (1) static sampler, (2) heap sampler, or (3) custom signature & shaders
    if (psoDesc.customRootSignature)
    {
        m_prootsignature = psoDesc.customRootSignature;
    }
    else
    {
        m_prootsignature = (psoDesc.samplerDescriptor.ptr) ? mDeviceResources->rootSignatureHeap.get() : mDeviceResources->rootSignatureStatic.get();
    }
    d3dDesc.pRootSignature = m_prootsignature.get();

    if (psoDesc.customVertexShader.pShaderBytecode)
    {
        d3dDesc.VS = psoDesc.customVertexShader;
    }
    else
    {
        d3dDesc.VS = (psoDesc.samplerDescriptor.ptr) ? s_DefaultVertexShaderByteCodeHeap : s_DefaultVertexShaderByteCodeStatic;
    }

    if (psoDesc.customPixelShader.pShaderBytecode)
    {
        d3dDesc.PS = psoDesc.customPixelShader;
    }
    else
    {
        d3dDesc.PS = (psoDesc.samplerDescriptor.ptr) ? s_DefaultPixelShaderByteCodeHeap : s_DefaultPixelShaderByteCodeStatic;
    }

    if (psoDesc.samplerDescriptor.ptr)
    {
        mSampler = psoDesc.samplerDescriptor;
    }

    ThrowIfFailed(device->CreateGraphicsPipelineState(
        &d3dDesc,
        IID_GRAPHICS_PPV_ARGS(mPSO.addressof())));

    SetDebugObjectName(mPSO.get(), L"SpriteBatch");
}

// Begins a batch of sprite drawing operations.
_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Impl::Begin(
    ID3D12GraphicsCommandList* commandList,
    SpriteSortMode sortMode,
    DirectX::FXMMATRIX transformMatrix)
{
    if (mInBeginEndPair)
    {
        DebugTrace("ERROR: Cannot nest Begin calls on a single SpriteBatch\n");
        throw std::exception("SpriteBatch::Begin");
    }

    mSortMode = sortMode;
    mTransformMatrix = transformMatrix;
    mCommandList = commandList;
    mSpriteCount = 0;

    if (sortMode == SpriteSortMode_Immediate)
    {
        PrepareForRendering();
    }

    mInBeginEndPair = true;
}

// Ends a batch of sprite drawing operations.
void SpriteBatch::Impl::End()
{
    if (!mInBeginEndPair)
    {
        DebugTrace("ERROR: Begin must be called before End\n");
        throw std::exception("SpriteBatch::End");
    }

    if (mSortMode != SpriteSortMode_Immediate)
    {
        PrepareForRendering();
        FlushBatch();
    }

    // Release this memory
    mVertexSegment.reset();

    // Break circular reference chains, in case the state lambda closed
    // over an object that holds a reference to this SpriteBatch.
    mCommandList = nullptr;
    mInBeginEndPair = false;
}


// Adds a single sprite to the queue.
_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Impl::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture,
    DirectX::XMUINT2 const& textureSize,
    DirectX::FXMVECTOR destination,
    RECT const* sourceRectangle,
    DirectX::FXMVECTOR color,
    DirectX::FXMVECTOR originRotationDepth,
    uint32_t flags)
{
    if (!mInBeginEndPair)
    {
        DebugTrace("ERROR: Begin must be called before Draw\n");
        throw std::exception("SpriteBatch::Draw");
    }

    if (!texture.ptr)
        throw std::exception("Invalid texture for Draw");

    // Get a pointer to the output sprite.
    if (mSpriteQueueCount >= mSpriteQueueArraySize)
    {
        GrowSpriteQueue();
    }

    SpriteInfo* sprite = &mSpriteQueue[mSpriteQueueCount];

    DirectX::XMVECTOR dest = destination;

    if (sourceRectangle)
    {
        // User specified an explicit source region.
        DirectX::XMVECTOR source = LoadRect(sourceRectangle);

        XMStoreFloat4A(&sprite->source, source);

        // If the destination size is relative to the source region, convert it to pixels.
        if (!(flags & SpriteInfo::DestSizeInPixels))
        {
            dest = DirectX::XMVectorPermute<0, 1, 6, 7>(dest, DirectX::XMVectorMultiply(dest, source)); // dest.zw *= source.zw
        }

        flags |= SpriteInfo::SourceInTexels | SpriteInfo::DestSizeInPixels;
    }
    else
    {
        // No explicit source region, so use the entire texture.
        static const DirectX::XMVECTORF32 wholeTexture = { { {0, 0, 1, 1} } };

        XMStoreFloat4A(&sprite->source, wholeTexture);
    }

    // Convert texture size
    DirectX::XMVECTOR textureSizeV = XMLoadUInt2(&textureSize);

    // Store sprite parameters.
    XMStoreFloat4A(&sprite->destination, dest);
    XMStoreFloat4A(&sprite->color, color);
    XMStoreFloat4A(&sprite->originRotationDepth, originRotationDepth);

    sprite->texture = texture;
    sprite->textureSize = textureSizeV;
    sprite->flags = flags;

    if (mSortMode == SpriteSortMode_Immediate)
    {
        // If we are in immediate mode, draw this sprite straight away.
        RenderBatch(texture, textureSizeV, &sprite, 1);
    }
    else
    {
        // Queue this sprite for later sorting and batched rendering.
        mSpriteQueueCount++;
    }
}


// Dynamically expands the array used to store pending sprite information.
void SpriteBatch::Impl::GrowSpriteQueue()
{
    // Grow by a factor of 2.
    size_t newSize = std::max(InitialQueueSize, mSpriteQueueArraySize * 2);

    // Allocate the new array.
    auto newArray = std::make_unique<SpriteInfo[]>(newSize);

    // Copy over any existing sprites.
    for (auto i = 0u; i < mSpriteQueueCount; i++)
    {
        newArray[i] = mSpriteQueue[i];
    }

    // Replace the previous array with the new one.
    mSpriteQueue = std::move(newArray);
    mSpriteQueueArraySize = newSize;

    // Clear any dangling SpriteInfo pointers left over from previous rendering.
    mSortedSprites.clear();
}


// Sets up D3D device state ready for drawing sprites.
void SpriteBatch::Impl::PrepareForRendering()
{
    auto commandList = mCommandList.get();

    // Set root signature
    commandList->SetGraphicsRootSignature(m_prootsignature.get());

    // Set render state
    commandList->SetPipelineState(mPSO.get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set the index buffer.
    commandList->IASetIndexBuffer(&mDeviceResources->indexBufferView);

    // Set the transform matrix.
    DirectX::XMMATRIX transformMatrix = (mRotation == DXGI_MODE_ROTATION_UNSPECIFIED)
        ? mTransformMatrix
        : (mTransformMatrix * GetViewportTransform(mRotation));

    mConstantBuffer = GraphicsMemory::Get(mDeviceResources->m_device).AllocateConstant(transformMatrix);
    commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, mConstantBuffer.GpuAddress());
}


// Sends queued sprites to the graphics device.
void SpriteBatch::Impl::FlushBatch()
{
    if (!mSpriteQueueCount)
        return;

    SortSprites();

    // Walk through the sorted sprite list, looking for adjacent entries that share a texture.
    D3D12_GPU_DESCRIPTOR_HANDLE batchTexture = {};
    DirectX::XMVECTOR batchTextureSize = {};
    size_t batchStart = 0;

    for (size_t pos = 0; pos < mSpriteQueueCount; pos++)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE texture = mSortedSprites[pos]->texture;
        assert(texture.ptr != 0);
        DirectX::XMVECTOR textureSize = mSortedSprites[pos]->textureSize;

        // Flush whenever the texture changes.
        if (texture != batchTexture)
        {
            if (pos > batchStart)
            {
                RenderBatch(batchTexture, batchTextureSize, &mSortedSprites[batchStart], pos - batchStart);
            }

            batchTexture = texture;
            batchTextureSize = textureSize;
            batchStart = pos;
        }
    }

    // Flush the final batch.
    RenderBatch(batchTexture, batchTextureSize, &mSortedSprites[batchStart], mSpriteQueueCount - batchStart);

    // Reset the queue.
    mSpriteQueueCount = 0;

    // When sorting is disabled, we persist mSortedSprites data from one batch to the next, to avoid
    // uneccessary work in GrowSortedSprites. But we never reuse these when sorting, because re-sorting
    // previously sorted items gives unstable ordering if some sprites have identical sort keys.
    if (mSortMode != SpriteSortMode_Deferred)
    {
        mSortedSprites.clear();
    }
}


// Sorts the array of queued sprites.
void SpriteBatch::Impl::SortSprites()
{
    // Fill the mSortedSprites vector.
    if (mSortedSprites.size() < mSpriteQueueCount)
    {
        GrowSortedSprites();
    }

    switch (mSortMode)
    {
    case SpriteSortMode_Texture:
        // Sort by texture.
        std::sort(mSortedSprites.begin(),
            mSortedSprites.begin() + static_cast<int32_t>(mSpriteQueueCount),
            [](SpriteInfo const* x, SpriteInfo const* y) noexcept -> bool
            {
                return x->texture < y->texture;
            });
        break;

    case SpriteSortMode_BackToFront:
        // Sort back to front.
        std::sort(mSortedSprites.begin(),
            mSortedSprites.begin() + static_cast<int32_t>(mSpriteQueueCount),
            [](SpriteInfo const* x, SpriteInfo const* y) noexcept -> bool
            {
                return x->originRotationDepth.w > y->originRotationDepth.w;
            });
        break;

    case SpriteSortMode_FrontToBack:
        // Sort front to back.
        std::sort(mSortedSprites.begin(),
            mSortedSprites.begin() + static_cast<int32_t>(mSpriteQueueCount),
            [](SpriteInfo const* x, SpriteInfo const* y) noexcept -> bool
            {
                return x->originRotationDepth.w < y->originRotationDepth.w;
            });
        break;

    default:
        break;
    }
}


// Populates the mSortedSprites vector with pointers to individual elements of the mSpriteQueue array.
void SpriteBatch::Impl::GrowSortedSprites()
{
    size_t previousSize = mSortedSprites.size();

    mSortedSprites.resize(mSpriteQueueCount);

    for (size_t i = previousSize; i < mSpriteQueueCount; i++)
    {
        mSortedSprites[i] = &mSpriteQueue[i];
    }
}


// Submits a batch of sprites to the GPU.
_Use_decl_annotations_
void SpriteBatch::Impl::RenderBatch(D3D12_GPU_DESCRIPTOR_HANDLE texture, DirectX::XMVECTOR textureSize, SpriteInfo const* const* sprites, size_t count)
{
    auto commandList = mCommandList.get();

    // Draw using the specified texture.
    // **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heap(s)
    commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);

    if (mSampler.ptr)
    {
        commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSampler, mSampler);
    }

    // Convert to vector format.
    DirectX::XMVECTOR inverseTextureSize = DirectX::XMVectorReciprocal(textureSize);

    while (count > 0)
    {
        // How many sprites do we want to draw?
        size_t batchSize = count;

        // How many sprites does the D3D vertex buffer have room for?
        size_t remainingSpace = MaxBatchSize - mSpriteCount;

        if (batchSize > remainingSpace)
        {
            if (remainingSpace < MinBatchSize)
            {
                // If we are out of room, or about to submit an excessively small batch, wrap back to the start of the vertex buffer.
                mSpriteCount = 0;

                batchSize = std::min(count, MaxBatchSize);
            }
            else
            {
                // Take however many sprites fit in what's left of the vertex buffer.
                batchSize = remainingSpace;
            }
        }

        // Allocate a new page of vertex memory if we're starting the batch
        if (mSpriteCount == 0)
        {
            mVertexSegment = GraphicsMemory::Get(mDeviceResources->m_device).Allocate(mVertexPageSize);
        }

        auto vertices = static_cast<VertexPositionColorTexture*>(mVertexSegment.Memory()) + mSpriteCount * VerticesPerSprite;

        // Generate sprite vertex data.
        for (auto i = 0u; i < batchSize; i++)
        {
            assert(i < count);
            _Analysis_assume_(i < count);
            RenderSprite(sprites[i], vertices, textureSize, inverseTextureSize);

            vertices += VerticesPerSprite;
        }

        // Set the vertex buffer view
        D3D12_VERTEX_BUFFER_VIEW vbv;
        size_t spriteVertexTotalSize = sizeof(VertexPositionColorTexture) * VerticesPerSprite;
        vbv.BufferLocation = mVertexSegment.GpuAddress() + (uint64_t(mSpriteCount) * uint64_t(spriteVertexTotalSize));
        vbv.StrideInBytes = sizeof(VertexPositionColorTexture);
        vbv.SizeInBytes = static_cast<uint32_t>(batchSize * spriteVertexTotalSize);
        commandList->IASetVertexBuffers(0, 1, &vbv);

        // Ok lads, the time has come for us draw ourselves some sprites!
        uint32_t indexCount = static_cast<uint32_t>(batchSize * IndicesPerSprite);

        commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

        // Advance the buffer position.
        mSpriteCount += batchSize;

        sprites += batchSize;
        count -= batchSize;
    }
}


// Generates vertex data for drawing a single sprite.
_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Impl::RenderSprite(SpriteInfo const* sprite, VertexPositionColorTexture* vertices, DirectX::FXMVECTOR textureSize, DirectX::FXMVECTOR inverseTextureSize) noexcept
{
    // Load sprite parameters into SIMD registers.
    DirectX::XMVECTOR source = XMLoadFloat4A(&sprite->source);
    DirectX::XMVECTOR destination = XMLoadFloat4A(&sprite->destination);
    DirectX::XMVECTOR color = XMLoadFloat4A(&sprite->color);
    DirectX::XMVECTOR originRotationDepth = XMLoadFloat4A(&sprite->originRotationDepth);

    float rotation = sprite->originRotationDepth.z;
    uint32_t flags = sprite->flags;

    // Extract the source and destination sizes into separate vectors.
    DirectX::XMVECTOR sourceSize = DirectX::XMVectorSwizzle<2, 3, 2, 3>(source);
    DirectX::XMVECTOR destinationSize = DirectX::XMVectorSwizzle<2, 3, 2, 3>(destination);

    // Scale the origin offset by source size, taking care to avoid overflow if the source region is zero.
    DirectX::XMVECTOR isZeroMask = DirectX::XMVectorEqual(sourceSize, DirectX::XMVectorZero());
    DirectX::XMVECTOR nonZeroSourceSize = DirectX::XMVectorSelect(sourceSize, DirectX::g_XMEpsilon, isZeroMask);

    DirectX::XMVECTOR origin = DirectX::XMVectorDivide(originRotationDepth, nonZeroSourceSize);

    // Convert the source region from texels to mod-1 texture coordinate format.
    if (flags & SpriteInfo::SourceInTexels)
    {
        source = DirectX::XMVectorMultiply(source, inverseTextureSize);
        sourceSize = DirectX::XMVectorMultiply(sourceSize, inverseTextureSize);
    }
    else
    {
        origin = DirectX::XMVectorMultiply(origin, inverseTextureSize);
    }

    // If the destination size is relative to the source region, convert it to pixels.
    if (!(flags & SpriteInfo::DestSizeInPixels))
    {
        destinationSize = DirectX::XMVectorMultiply(destinationSize, textureSize);
    }

    // Compute a 2x2 rotation matrix.
    DirectX::XMVECTOR rotationMatrix1;
    DirectX::XMVECTOR rotationMatrix2;

    if (rotation != 0)
    {
        float sin, cos;

        DirectX::XMScalarSinCos(&sin, &cos, rotation);

        DirectX::XMVECTOR sinV = DirectX::XMLoadFloat(&sin);
        DirectX::XMVECTOR cosV = DirectX::XMLoadFloat(&cos);

        rotationMatrix1 = DirectX::XMVectorMergeXY(cosV, sinV);
        rotationMatrix2 = DirectX::XMVectorMergeXY(DirectX::XMVectorNegate(sinV), cosV);
    }
    else
    {
        rotationMatrix1 = DirectX::g_XMIdentityR0;
        rotationMatrix2 = DirectX::g_XMIdentityR1;
    }

    // The four corner vertices are computed by transforming these unit-square positions.
    static DirectX::XMVECTORF32 cornerOffsets[VerticesPerSprite] =
    {
        { { { 0, 0, 0, 0 } } },
        { { { 1, 0, 0, 0 } } },
        { { { 0, 1, 0, 0 } } },
        { { { 1, 1, 0, 0 } } },
    };

    // Tricksy alert! Texture coordinates are computed from the same cornerOffsets
    // table as vertex positions, but if the sprite is mirrored, this table
    // must be indexed in a different order. This is done as follows:
    //
    //    position = cornerOffsets[i]
    //    texcoord = cornerOffsets[i ^ SpriteEffects]

    static_assert(SpriteEffects_FlipHorizontally == 1 &&
        SpriteEffects_FlipVertically == 2, "If you change these enum values, the mirroring implementation must be updated to match");

    const uint32_t mirrorBits = flags & 3u;

    // Generate the four output vertices.
    for (auto i = 0u; i < VerticesPerSprite; i++)
    {
        // Calculate position.
        DirectX::XMVECTOR cornerOffset = DirectX::XMVectorMultiply(DirectX::XMVectorSubtract(cornerOffsets[i], origin), destinationSize);

        // Apply 2x2 rotation matrix.
        DirectX::XMVECTOR position1 = DirectX::XMVectorMultiplyAdd(DirectX::XMVectorSplatX(cornerOffset), rotationMatrix1, destination);
        DirectX::XMVECTOR position2 = DirectX::XMVectorMultiplyAdd(DirectX::XMVectorSplatY(cornerOffset), rotationMatrix2, position1);

        // Set z = depth.
        DirectX::XMVECTOR position = DirectX::XMVectorPermute<0, 1, 7, 6>(position2, originRotationDepth);

        // Write position as a Float4, even though VertexPositionColor::position is an DirectX::XMFLOAT3.
        // This is faster, and harmless as we are just clobbering the first element of the
        // following color field, which will immediately be overwritten with its correct value.
        XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&vertices[i].position), position);

        // Write the color.
        XMStoreFloat4(&vertices[i].color, color);

        // Compute and write the texture coordinate.
        DirectX::XMVECTOR textureCoordinate = DirectX::XMVectorMultiplyAdd(cornerOffsets[static_cast<uint32_t>(i) ^ mirrorBits], sourceSize, source);

        XMStoreFloat2(&vertices[i].textureCoordinate, textureCoordinate);
    }
}


// Generates a viewport transform matrix for rendering sprites using x-right y-down screen pixel coordinates.
DirectX::XMMATRIX SpriteBatch::Impl::GetViewportTransform(_In_ DXGI_MODE_ROTATION rotation)
{
    if (!mSetViewport)
    {
        DebugTrace("ERROR: SpriteBatch requires viewport information via SetViewport\n");
        throw std::exception("Viewport not set.");
    }

    // Compute the matrix.
    float xScale = (mViewPort.Width > 0) ? 2.0f / mViewPort.Width : 0.0f;
    float yScale = (mViewPort.Height > 0) ? 2.0f / mViewPort.Height : 0.0f;

    switch (rotation)
    {
    case DXGI_MODE_ROTATION_ROTATE90:
        return DirectX::XMMATRIX
            (
                0, -yScale, 0, 0,
                -xScale, 0, 0, 0,
                0, 0, 1, 0,
                1, 1, 0, 1
                );

    case DXGI_MODE_ROTATION_ROTATE270:
        return DirectX::XMMATRIX
            (
                0, yScale, 0, 0,
                xScale, 0, 0, 0,
                0, 0, 1, 0,
                -1, -1, 0, 1
                );

    case DXGI_MODE_ROTATION_ROTATE180:
        return DirectX::XMMATRIX
            (
                -xScale, 0, 0, 0,
                0, yScale, 0, 0,
                0, 0, 1, 0,
                1, -1, 0, 1
                );

    default:
        return DirectX::XMMATRIX
            (
                xScale, 0, 0, 0,
                0, -yScale, 0, 0,
                0, 0, 1, 0,
                -1, 1, 0, 1
                );
    }
}


// Public constructor.
_Use_decl_annotations_
SpriteBatch::SpriteBatch(ID3D12Device* device,
    ResourceUploadBatch& upload,
    const SpriteBatchPipelineStateDescription& psoDesc,
    const D3D12_VIEWPORT* viewport)
    : pimpl(std::make_unique<Impl>(device, upload, psoDesc, viewport))
{
}


// Move constructor.
SpriteBatch::SpriteBatch(SpriteBatch&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
}


// Move assignment.
SpriteBatch& SpriteBatch::operator= (SpriteBatch&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    return *this;
}


// Public destructor.
SpriteBatch::~SpriteBatch()
{
}

_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Begin(
    ID3D12GraphicsCommandList* commandList,
    SpriteSortMode sortMode,
    DirectX::FXMMATRIX transformMatrix)
{
    pimpl->Begin(commandList, sortMode, transformMatrix);
}


_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Begin(
    ID3D12GraphicsCommandList* commandList,
    D3D12_GPU_DESCRIPTOR_HANDLE sampler,
    SpriteSortMode sortMode,
    DirectX::FXMMATRIX transformMatrix)
{
    if (!sampler.ptr)
        throw std::exception("Invalid heap-based sampler for Begin");

    if (!pimpl->mSampler.ptr)
    {
        DebugTrace("ERROR: sampler version of Begin requires SpriteBatch was created with a heap-based sampler\n");
        throw std::exception("SpriteBatch::Begin");
    }

    pimpl->mSampler = sampler;

    pimpl->Begin(commandList, sortMode, transformMatrix);
}


void SpriteBatch::End()
{
    pimpl->End();
}


void XM_CALLCONV SpriteBatch::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture,
    DirectX::XMUINT2 const& textureSize,
    DirectX::XMFLOAT2 const& position,
    DirectX::FXMVECTOR color)
{
    DirectX::XMVECTOR destination = DirectX::XMVectorPermute<0, 1, 4, 5>(XMLoadFloat2(&position), DirectX::g_XMOne); // x, y, 1, 1

    pimpl->Draw(texture, textureSize, destination, nullptr, color, DirectX::g_XMZero, 0);
}


_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture,
    DirectX::XMUINT2 const& textureSize,
    DirectX::XMFLOAT2 const& position, 
    RECT const* sourceRectangle,
    DirectX::FXMVECTOR color,
    float rotation,
    DirectX::XMFLOAT2 const& origin,
    float scale,
    SpriteEffects effects,
    float layerDepth)
{
    DirectX::XMVECTOR destination = DirectX::XMVectorPermute<0, 1, 4, 4>(XMLoadFloat2(&position), DirectX::XMLoadFloat(&scale)); // x, y, scale, scale

    DirectX::XMVECTOR originRotationDepth = DirectX::XMVectorSet(origin.x, origin.y, rotation, layerDepth);

    pimpl->Draw(texture, textureSize, destination, sourceRectangle, color, originRotationDepth, static_cast<uint32_t>(effects));
}


_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture, 
    DirectX::XMUINT2 const& textureSize,
    DirectX::XMFLOAT2 const& position,
    RECT const* sourceRectangle,
    DirectX::FXMVECTOR color,
    float rotation,
    DirectX::XMFLOAT2 const& origin,
    DirectX::XMFLOAT2 const& scale,
    SpriteEffects effects,
    float layerDepth)
{
    DirectX::XMVECTOR destination = DirectX::XMVectorPermute<0, 1, 4, 5>(XMLoadFloat2(&position), XMLoadFloat2(&scale)); // x, y, scale.x, scale.y

    DirectX::XMVECTOR originRotationDepth = DirectX::XMVectorSet(origin.x, origin.y, rotation, layerDepth);

    pimpl->Draw(texture, textureSize, destination, sourceRectangle, color, originRotationDepth, static_cast<uint32_t>(effects));
}


void XM_CALLCONV SpriteBatch::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture, DirectX::XMUINT2 const& textureSize, DirectX::FXMVECTOR position, DirectX::FXMVECTOR color)
{
    DirectX::XMVECTOR destination = DirectX::XMVectorPermute<0, 1, 4, 5>(position, DirectX::g_XMOne); // x, y, 1, 1

    pimpl->Draw(texture, textureSize, destination, nullptr, color, DirectX::g_XMZero, 0);
}


_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture,
    DirectX::XMUINT2 const& textureSize,
    DirectX::FXMVECTOR position, 
    RECT const* sourceRectangle,
    DirectX::FXMVECTOR color,
    float rotation,
    DirectX::FXMVECTOR origin,
    float scale,
    SpriteEffects effects,
    float layerDepth)
{
    DirectX::XMVECTOR destination = DirectX::XMVectorPermute<0, 1, 4, 4>(position, DirectX::XMLoadFloat(&scale)); // x, y, scale, scale

    DirectX::XMVECTOR rotationDepth = DirectX::XMVectorMergeXY(DirectX::XMVectorReplicate(rotation), DirectX::XMVectorReplicate(layerDepth));

    DirectX::XMVECTOR originRotationDepth = DirectX::XMVectorPermute<0, 1, 4, 5>(origin, rotationDepth);

    pimpl->Draw(texture, textureSize, destination, sourceRectangle, color, originRotationDepth, static_cast<uint32_t>(effects));
}


_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture,
    DirectX::XMUINT2 const& textureSize,
    DirectX::FXMVECTOR position,
    RECT const* sourceRectangle,
    DirectX::FXMVECTOR color,
    float rotation,
    DirectX::FXMVECTOR origin,
    DirectX::GXMVECTOR scale,
    SpriteEffects effects,
    float layerDepth)
{
    DirectX::XMVECTOR destination = DirectX::XMVectorPermute<0, 1, 4, 5>(position, scale); // x, y, scale.x, scale.y

    DirectX::XMVECTOR rotationDepth = DirectX::XMVectorMergeXY(DirectX::XMVectorReplicate(rotation), DirectX::XMVectorReplicate(layerDepth));

    DirectX::XMVECTOR originRotationDepth = DirectX::XMVectorPermute<0, 1, 4, 5>(origin, rotationDepth);

    pimpl->Draw(texture, textureSize, destination, sourceRectangle, color, originRotationDepth, static_cast<uint32_t>(effects));
}


void XM_CALLCONV SpriteBatch::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture, 
    DirectX::XMUINT2 const& textureSize,
    RECT const& destinationRectangle,
    DirectX::FXMVECTOR color)
{
    DirectX::XMVECTOR destination = LoadRect(&destinationRectangle); // x, y, w, h

    pimpl->Draw(texture, textureSize, destination, nullptr, color, DirectX::g_XMZero, Impl::SpriteInfo::DestSizeInPixels);
}


_Use_decl_annotations_
void XM_CALLCONV SpriteBatch::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture,
    DirectX::XMUINT2 const& textureSize,
    RECT const& destinationRectangle,
    RECT const* sourceRectangle,
    DirectX::FXMVECTOR color,
    float rotation,
    DirectX::XMFLOAT2 const& origin, 
    SpriteEffects effects, 
    float layerDepth)
{
    DirectX::XMVECTOR destination = LoadRect(&destinationRectangle); // x, y, w, h

    DirectX::XMVECTOR originRotationDepth = DirectX::XMVectorSet(origin.x, origin.y, rotation, layerDepth);

    pimpl->Draw(texture, textureSize, destination, sourceRectangle, color, originRotationDepth, static_cast<uint32_t>(effects) | Impl::SpriteInfo::DestSizeInPixels);
}


void SpriteBatch::SetRotation(DXGI_MODE_ROTATION mode)
{
    pimpl->mRotation = mode;
}


DXGI_MODE_ROTATION SpriteBatch::GetRotation() const noexcept
{
    return pimpl->mRotation;
}


void SpriteBatch::SetViewport(const D3D12_VIEWPORT& viewPort)
{
    pimpl->mSetViewport = true;
    pimpl->mViewPort = viewPort;
}
