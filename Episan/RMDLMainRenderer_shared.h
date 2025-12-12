//
//  ShaderTypes.h
//  Episan
//
//  Created by Rémy on 15/11/2025.
//

#ifndef RMDLMAINRENDERER_SHARED_H
#define RMDLMAINRENDERER_SHARED_H

#include <simd/simd.h>

struct RMDLCameraUniforms
{
    simd::float4x4      viewMatrix;
    simd::float4x4      projectionMatrix;
    simd::float4x4      viewProjectionMatrix;
    simd::float4x4      invOrientationProjectionMatrix;
    simd::float4x4      invViewProjectionMatrix;
    simd::float4x4      invProjectionMatrix;
    simd::float4x4      invViewMatrix;
    simd::float4        frustumPlanes[6];
};

struct RMDLUniforms
{
    RMDLCameraUniforms  cameraUniforms;
    RMDLCameraUniforms  shadowCameraUniforms[3];

    // Mouse state: x, y = position in pixels; z = buttons
    simd::float3        mouseState;
    simd::float2        invScreenSize;
    float               projectionYScale;
    float               brushSize;

    float               ambientOcclusionContrast;
    float               ambientOcclusionScale;
    float               ambientLightScale;
#if !USE_CONST_GAME_TIME
    float               gameTime;
#endif
    float               frameTime;  // TODO. this doesn't appear to be initialized until UpdateCpuUniforms. OK?
};

struct Plane
{
    simd::float3        normal = { 0.f, 1.f, 0.f };
    float               distance = 0.f;
};

struct Frustum
{
    Plane               topFace;
    Plane               bottomFace;
    Plane               rightFace;
    Plane               leftFace;
    Plane               farFace;
    Plane               nearFace;
};

struct RMDLObjVertex
{
    simd::float3    position;
    simd::float3    normal;
    simd::float3    color;
};

typedef enum VertexAttributes
{
    VertexAttributePosition  = 0,
    VertexAttributeTexcoord  = 1,
    VertexAttributeNormal    = 2,
    VertexAttributeTangent   = 3,
    VertexAttributeBitangent = 4
}   VertexAttributes;

typedef enum BufferIndex
{
    BufferIndexMeshPositions     = 0,
    BufferIndexMeshGenerics      = 1,
    BufferIndexFrameData         = 2,
    BufferIndexLightsData        = 3,
    BufferIndexLightsPosition    = 4,
    BufferIndexFlatColor         = 0,
    BufferIndexDepthRange        = 0,
}   BufferIndex;

typedef enum TextureIndex
{
    TextureIndexBaseColor = 0,
    TextureIndexSpecular  = 1,
    TextureIndexNormal    = 2,
    TextureIndexShadow    = 3,
    TextureIndexAlpha     = 4,
    NumMeshTextures = TextureIndexNormal + 1
}   TextureIndex;

typedef enum RenderTargetIndex
{
    RenderTargetLighting  = 0,
    RenderTargetAlbedo    = 1,
    RenderTargetNormal    = 2,
    RenderTargetDepth     = 3
}   RenderTargetIndex;

typedef struct
{
    simd_float2 position;
    simd_float4 color;
} VertexData;

struct JDLVState
{
    uint32_t width;
    uint32_t height;
};

struct TextVertex
{
    simd::float2 pos;
    simd::float2 uv;
};

struct TextVertexC
{
    simd::float2 position;  // Position en NDC (-1 à 1)
    simd::float2 texCoord;  // Coordonnées UV dans l'atlas
};


#endif /* RMDLMAINRENDERER_SHARED_H */

