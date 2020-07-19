//--------------------------------------------------------------------------------------
// File: EffectCommon.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "effectcommon.h"
#include "DemandCreate.h"
#include "resourceuploadbatch.h"

using namespace directxtk;
// using Microsoft::WRL::ComPtr;


// IEffectMatrices default method
void XM_CALLCONV IEffectMatrices::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
    SetWorld(world);
    SetView(view);
    SetProjection(projection);
}


// Constructor initializes default matrix values.
EffectMatrices::EffectMatrices() noexcept
{
    DirectX::XMMATRIX id = DirectX::XMMatrixIdentity();
    world = id;
    view = id;
    projection = id;
    worldView = id;
}


// Lazily recomputes the combined world+view+projection matrix.
_Use_decl_annotations_ void EffectMatrices::SetConstants(int32_t& dirtyFlags, DirectX::XMMATRIX& worldViewProjConstant)
{
    if (dirtyFlags & EffectDirtyFlags::WorldViewProj)
    {
        worldView = XMMatrixMultiply(world, view);

        worldViewProjConstant = XMMatrixTranspose(XMMatrixMultiply(worldView, projection));
                
        dirtyFlags &= ~EffectDirtyFlags::WorldViewProj;
        dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
    }
}


// Constructor initializes default fog settings.
EffectFog::EffectFog() noexcept :
    enabled(false),
    start(0),
    end(1.f)
{
}


// Lazily recomputes the derived vector used by shader fog calculations.
_Use_decl_annotations_
void XM_CALLCONV EffectFog::SetConstants(int32_t& dirtyFlags, DirectX::FXMMATRIX worldView, DirectX::XMVECTOR& fogVectorConstant)
{
    if (enabled)
    {
        if (dirtyFlags & (EffectDirtyFlags::FogVector | EffectDirtyFlags::FogEnable))
        {
            if (start == end)
            {
                // Degenerate case: force everything to 100% fogged if start and end are the same.
                static const DirectX::XMVECTORF32 fullyFogged = { { { 0, 0, 0, 1 } } };

                fogVectorConstant = fullyFogged;
            }
            else
            {
                // We want to transform vertex positions into view space, take the resulting
                // Z value, then scale and offset according to the fog start/end distances.
                // Because we only care about the Z component, the shader can do all this
                // with a single dot product, using only the Z row of the world+view matrix.
        
                // _13, _23, _33, _43
                DirectX::XMVECTOR worldViewZ = DirectX::XMVectorMergeXY(
					DirectX::XMVectorMergeZW(worldView.r[0], worldView.r[2]),
					DirectX::XMVectorMergeZW(worldView.r[1], worldView.r[3]));

                // 0, 0, 0, fogStart
                DirectX::XMVECTOR wOffset = DirectX::XMVectorSwizzle<1, 2, 3, 0>(DirectX::XMLoadFloat(&start));

                // (worldViewZ + wOffset) / (start - end);
                fogVectorConstant = DirectX::XMVectorDivide(DirectX::XMVectorAdd(worldViewZ, wOffset), DirectX::XMVectorReplicate(start - end));
            }

            dirtyFlags &= ~(EffectDirtyFlags::FogVector | EffectDirtyFlags::FogEnable);
            dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
        }
    }
    else
    {
        // When fog is disabled, make sure the fog vector is reset to zero.
        if (dirtyFlags & EffectDirtyFlags::FogEnable)
        {
            fogVectorConstant = DirectX::g_XMZero;

            dirtyFlags &= ~EffectDirtyFlags::FogEnable;
            dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
        }
    }
}


// Constructor initializes default material color settings.
EffectColor::EffectColor() noexcept :
    diffuseColor(DirectX::g_XMOne),
    alpha(1.f)
{
}


// Lazily recomputes the material color parameter for shaders that do not support realtime lighting.
void EffectColor::SetConstants(_Inout_ int32_t& dirtyFlags, _Inout_ DirectX::XMVECTOR& diffuseColorConstant)
{
    if (dirtyFlags & EffectDirtyFlags::MaterialColor)
    {
        DirectX::XMVECTOR alphaVector = DirectX::XMVectorReplicate(alpha);

        // xyz = diffuse * alpha, w = alpha.
        diffuseColorConstant = DirectX::XMVectorSelect(alphaVector, DirectX::XMVectorMultiply(diffuseColor, alphaVector), DirectX::g_XMSelect1110);

        dirtyFlags &= ~EffectDirtyFlags::MaterialColor;
        dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
    }
}


// Constructor initializes default light settings.
EffectLights::EffectLights() noexcept :
    emissiveColor{},
    ambientLightColor{},
    lightEnabled{},
    lightDiffuseColor{},
    lightSpecularColor{}
{
    for (int32_t i = 0; i < MaxDirectionalLights; i++)
    {
        lightEnabled[i] = (i == 0);
        lightDiffuseColor[i] = DirectX::g_XMOne;
    }
}


#ifdef _PREFAST_
#pragma prefast(push)
#pragma prefast(disable:22103, "PREFAST doesn't understand buffer is bounded by a static const value even with SAL" )
#endif

// Initializes constant buffer fields to match the current lighting state.
_Use_decl_annotations_ void EffectLights::InitializeConstants(DirectX::XMVECTOR& specularColorAndPowerConstant, DirectX::XMVECTOR* lightDirectionConstant, DirectX::XMVECTOR* lightDiffuseConstant, DirectX::XMVECTOR* lightSpecularConstant) const
{
    static const DirectX::XMVECTORF32 defaultSpecular = { { { 1, 1, 1, 16 } } };
    static const DirectX::XMVECTORF32 defaultLightDirection = { { { 0, -1, 0, 0 } } };
    
    specularColorAndPowerConstant = defaultSpecular;

    for (int32_t i = 0; i < MaxDirectionalLights; i++)
    {
        lightDirectionConstant[i] = defaultLightDirection;

        lightDiffuseConstant[i]  = lightEnabled[i] ? lightDiffuseColor[i]  : DirectX::g_XMZero;
        lightSpecularConstant[i] = lightEnabled[i] ? lightSpecularColor[i] : DirectX::g_XMZero;
    }
}

#ifdef _PREFAST_
#pragma prefast(pop)
#endif


// Lazily recomputes derived parameter values used by shader lighting calculations.
_Use_decl_annotations_
void EffectLights::SetConstants(int32_t& dirtyFlags, EffectMatrices const& matrices, DirectX::XMMATRIX& worldConstant, DirectX::XMVECTOR worldInverseTransposeConstant[3], DirectX::XMVECTOR& eyePositionConstant, DirectX::XMVECTOR& diffuseColorConstant, DirectX::XMVECTOR& emissiveColorConstant, bool lightingEnabled)
{
    if (lightingEnabled)
    {
        // World inverse transpose matrix.
        if (dirtyFlags & EffectDirtyFlags::WorldInverseTranspose)
        {
            worldConstant = XMMatrixTranspose(matrices.world);

            DirectX::XMMATRIX worldInverse = XMMatrixInverse(nullptr, matrices.world);

            worldInverseTransposeConstant[0] = worldInverse.r[0];
            worldInverseTransposeConstant[1] = worldInverse.r[1];
            worldInverseTransposeConstant[2] = worldInverse.r[2];

            dirtyFlags &= ~EffectDirtyFlags::WorldInverseTranspose;
            dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
        }

        // Eye position vector.
        if (dirtyFlags & EffectDirtyFlags::EyePosition)
        {
            DirectX::XMMATRIX viewInverse = XMMatrixInverse(nullptr, matrices.view);
        
            eyePositionConstant = viewInverse.r[3];

            dirtyFlags &= ~EffectDirtyFlags::EyePosition;
            dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
        }
    }

    // Material color parameters. The desired lighting model is:
    //
    //     ((ambientLightColor + sum(diffuse directional light)) * diffuseColor) + emissiveColor
    //
    // When lighting is disabled, ambient and directional lights are ignored, leaving:
    //
    //     diffuseColor + emissiveColor
    //
    // For the lighting disabled case, we can save one shader instruction by precomputing
    // diffuse+emissive on the CPU, after which the shader can use diffuseColor directly,
    // ignoring its emissive parameter.
    //
    // When lighting is enabled, we can merge the ambient and emissive settings. If we
    // set our emissive parameter to emissive+(ambient*diffuse), the shader no longer
    // needs to bother adding the ambient contribution, simplifying its computation to:
    //
    //     (sum(diffuse directional light) * diffuseColor) + emissiveColor
    //
    // For futher optimization goodness, we merge material alpha with the diffuse
    // color parameter, and premultiply all color values by this alpha.

    if (dirtyFlags & EffectDirtyFlags::MaterialColor)
    {
        DirectX::XMVECTOR diffuse = diffuseColor;
        DirectX::XMVECTOR alphaVector = DirectX::XMVectorReplicate(alpha);

        if (lightingEnabled)
        {
            // Merge emissive and ambient light contributions.
            // (emissiveColor + ambientLightColor * diffuse) * alphaVector;
            emissiveColorConstant = DirectX::XMVectorMultiply(DirectX::XMVectorMultiplyAdd(ambientLightColor, diffuse, emissiveColor), alphaVector);
        }
        else
        {
            // Merge diffuse and emissive light contributions.
            diffuse = DirectX::XMVectorAdd(diffuse, emissiveColor);
        }

        // xyz = diffuse * alpha, w = alpha.
        diffuseColorConstant = DirectX::XMVectorSelect(alphaVector, DirectX::XMVectorMultiply(diffuse, alphaVector), DirectX::g_XMSelect1110);

        dirtyFlags &= ~EffectDirtyFlags::MaterialColor;
        dirtyFlags |= EffectDirtyFlags::ConstantBuffer;
    }
}


#ifdef _PREFAST_
#pragma prefast(push)
#pragma prefast(disable:26015, "PREFAST doesn't understand that ValidateLightIndex bounds whichLight" )
#endif

// Helper for turning one of the directional lights on or off.
_Use_decl_annotations_ int32_t EffectLights::SetLightEnabled(int32_t whichLight, bool value, DirectX::XMVECTOR* lightDiffuseConstant, DirectX::XMVECTOR* lightSpecularConstant)
{
    ValidateLightIndex(whichLight);

    if (lightEnabled[whichLight] == value)
        return 0;

    lightEnabled[whichLight] = value;

    if (value)
    {
        // If this light is now on, store its color in the constant buffer.
        lightDiffuseConstant[whichLight] = lightDiffuseColor[whichLight];
        lightSpecularConstant[whichLight] = lightSpecularColor[whichLight];
    }
    else
    {
        // If the light is off, reset constant buffer colors to zero.
        lightDiffuseConstant[whichLight] = DirectX::g_XMZero;
        lightSpecularConstant[whichLight] = DirectX::g_XMZero;
    }

    return EffectDirtyFlags::ConstantBuffer;
}


// Helper for setting diffuse color of one of the directional lights.
_Use_decl_annotations_
int32_t XM_CALLCONV EffectLights::SetLightDiffuseColor(int32_t whichLight, DirectX::FXMVECTOR value, DirectX::XMVECTOR* lightDiffuseConstant)
{
    ValidateLightIndex(whichLight);

    // Locally store the new color.
    lightDiffuseColor[whichLight] = value;

    // If this light is currently on, also update the constant buffer.
    if (lightEnabled[whichLight])
    {
        lightDiffuseConstant[whichLight] = value;
        
        return EffectDirtyFlags::ConstantBuffer;
    }

    return 0;
}


// Helper for setting specular color of one of the directional lights.
_Use_decl_annotations_
int32_t XM_CALLCONV EffectLights::SetLightSpecularColor(int32_t whichLight, DirectX::FXMVECTOR value, DirectX::XMVECTOR* lightSpecularConstant)
{
    ValidateLightIndex(whichLight);

    // Locally store the new color.
    lightSpecularColor[whichLight] = value;

    // If this light is currently on, also update the constant buffer.
    if (lightEnabled[whichLight])
    {
        lightSpecularConstant[whichLight] = value;

        return EffectDirtyFlags::ConstantBuffer;
    }
    
    return 0;
}

#ifdef _PREFAST_
#pragma prefast(pop)
#endif


// Parameter validation helper.
void EffectLights::ValidateLightIndex(int32_t whichLight)
{
    if (whichLight < 0 || whichLight >= MaxDirectionalLights)
    {
        throw std::out_of_range("whichLight parameter out of range");
    }
}


// Activates the default lighting rig (key, fill, and back lights).
void EffectLights::EnableDefaultLighting(_In_ IEffectLights* effect)
{
    static const DirectX::XMVECTORF32 defaultDirections[MaxDirectionalLights] =
    {
        { { { -0.5265408f, -0.5735765f, -0.6275069f, 0 } } },
        { { {  0.7198464f,  0.3420201f,  0.6040227f, 0 } } },
        { { {  0.4545195f, -0.7660444f,  0.4545195f, 0 } } },
    };

    static const DirectX::XMVECTORF32 defaultDiffuse[MaxDirectionalLights] =
    {
        { { { 1.0000000f, 0.9607844f, 0.8078432f, 0 } } },
        { { { 0.9647059f, 0.7607844f, 0.4078432f, 0 } } },
        { { { 0.3231373f, 0.3607844f, 0.3937255f, 0 } } },
    };

    static const DirectX::XMVECTORF32 defaultSpecular[MaxDirectionalLights] =
    {
        { { { 1.0000000f, 0.9607844f, 0.8078432f, 0 } } },
        { { { 0.0000000f, 0.0000000f, 0.0000000f, 0 } } },
        { { { 0.3231373f, 0.3607844f, 0.3937255f, 0 } } },
    };

    static const DirectX::XMVECTORF32 defaultAmbient = { { { 0.05333332f, 0.09882354f, 0.1819608f, 0 } } };

    effect->SetAmbientLightColor(defaultAmbient);

    for (int32_t i = 0; i < MaxDirectionalLights; i++)
    {
        effect->SetLightEnabled(i, true);
        effect->SetLightDirection(i, defaultDirections[i]);
        effect->SetLightDiffuseColor(i, defaultDiffuse[i]);
        effect->SetLightSpecularColor(i, defaultSpecular[i]);
    }
}


// Gets or lazily creates the specified root signature.
ID3D12RootSignature* EffectDeviceResources::DemandCreateRootSig(_Inout_ wil::com_ptr<ID3D12RootSignature>& rootSig, D3D12_ROOT_SIGNATURE_DESC const& desc)
{
    return DemandCreate(rootSig, m_mutex, [&](ID3D12RootSignature** pResult) noexcept -> HRESULT
    {
        HRESULT hr = CreateRootSignature(m_pdevice.get(), &desc, pResult);

        if (SUCCEEDED(hr))
            SetDebugObjectName(*pResult, L"DirectXTK:Effect");

        return hr;
    });
}
