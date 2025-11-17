//
//  Shaders.metal
//  Episan
//
//  Created by Rémy on 15/11/2025.
//

#include <metal_stdlib>
#include <simd/simd.h>

#import "RMDLMainRenderer_shared.h"

using namespace metal;

typedef struct
{
    float3 position [[attribute(VertexAttributePosition)]];
    float2 texCoord [[attribute(VertexAttributeTexcoord)]];
} Vertex;

typedef struct
{
    float4 position [[position]];
    float2 texCoord;
} ColorInOut;

vertex ColorInOut vertexShaderCube(Vertex in [[stage_in]],
                                   constant RMDLCameraUniforms & uniforms [[ buffer(BufferIndexMeshPositions) ]])
{
    ColorInOut out;

    float4 position = float4(in.position, 1.0);
    out.position = uniforms.projectionMatrix * uniforms.viewProjectionMatrix * position;
    out.texCoord = in.texCoord;

    return out;
}

fragment float4 fragmentShaderCube(ColorInOut in [[stage_in]],
                               constant RMDLCameraUniforms & uniforms [[ buffer(BufferIndexMeshPositions) ]],
                               texture2d<half> colorMap     [[ texture(TextureIndexBaseColor) ]])
{
    constexpr sampler colorSampler(mip_filter::linear,
                                   mag_filter::linear,
                                   min_filter::linear);

    half4 colorSample   = colorMap.sample(colorSampler, in.texCoord.xy);

    return float4(colorSample);
}
