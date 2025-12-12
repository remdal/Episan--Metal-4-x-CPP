//
//  Textes.metal
//  Episan
//
//  Created by Rémy on 10/12/2025.
//

#include <metal_stdlib>
using namespace metal;

#include "RMDLMainRenderer_shared.h"

struct VSOut
{
    float4 position [[position]];
    float2 uv;
};

vertex VSOut textVS(uint id [[vertex_id]],
                     const device TextVertex* v [[buffer(0)]])
{
    VSOut o;
    o.position = float4(v[id].pos, 0.0, 1.0);
    o.uv = v[id].uv;
    return o;
}

fragment float4 textFS(VSOut in [[stage_in]],
                       texture2d<float> fontTex [[texture(0)]])
{
    constexpr sampler s(filter::linear, address::clamp_to_edge);
    float4 c = fontTex.sample(s, in.uv);
    return c;
}

struct VertexIn {
    float2 position [[attribute(0)]];
    float2 texCoord [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float2 texCoord;
};

vertex VertexOut textVertexShader(VertexIn in [[stage_in]]) {
    VertexOut out;
    out.position = float4(in.position, 0.0, 1.0);
    out.texCoord = in.texCoord;
    return out;
}

fragment float4 textFragmentShader(VertexOut in [[stage_in]],
                                   texture2d<float> fontTexture [[texture(0)]]) {
    constexpr sampler texSampler(filter::linear);
    return fontTexture.sample(texSampler, in.texCoord);
}





/*  équivalent Metal    */

// Structures de données
struct FrameData {
    float4x4 projectionMx;
};

struct InstancePositions {
    float4 position;
};

struct VertexInn {
    float2 position [[attribute(0)]];
    float2 texcoord [[attribute(1)]];
};

struct VertexOutt {
    float4 position [[position]];
    float4 color;
    float2 uv0;
    float arrayIndex [[flat]];
};

// Shader Vertex
vertex VertexOutt MainVS(
    uint instanceID [[instance_id]],
    const device VertexInn* vertices [[buffer(0)]],
    const device InstancePositions* instancePositions [[buffer(1)]],
    constant FrameData& frameData [[buffer(2)]],
    uint vid [[vertex_id]])
{
    VertexOutt out;
    
    VertexInn vin = vertices[vid];
    float3 instancePos = instancePositions[instanceID].position.xyz;
    
    // Transformation
    float4 pos = float4(vin.position.xy + instancePos.xy, 0.0, 1.0);
    out.position = frameData.projectionMx * pos;
    
    out.color = float4(1.0);
    out.uv0 = vin.texcoord;
    out.arrayIndex = instancePos.z;
    
    return out;
}

fragment half4 MainFS(
    VertexOutt in [[stage_in]],
    texture2d_array<half> texture [[texture(0)]],
    sampler samp [[sampler(0)]])
{
    half4 texel = texture.sample(samp, in.uv0, in.arrayIndex);
    texel.rgb *= texel.a;
    return texel;
}


/*  Claude  */

// Structure de sortie du vertex shader
struct TextVertexOut
{
    float4 position [[position]];
    float2 texCoord;
};

// Vertex Shader pour le texte
vertex TextVertexOut textVSc(
    uint vertexID [[vertex_id]],
    device const TextVertexC* vertices [[buffer(0)]])
{
    TextVertexOut out;
    
    TextVertexC in = vertices[vertexID];
    
    // Position directement en NDC (Normalized Device Coordinates)
    out.position = float4(in.position, 0.0, 1.0);
    out.texCoord = in.texCoord;
    
    return out;
}

// Fragment Shader pour le texte
fragment float4 textFSc(
    TextVertexOut in [[stage_in]],
    texture2d<float> fontTexture [[texture(0)]])
{
    constexpr sampler texSampler(
        mag_filter::linear,
        min_filter::linear,
        address::clamp_to_edge
    );
    
    // Sample la texture du font atlas
    float4 textureColor = fontTexture.sample(texSampler, in.texCoord);
    
    // Les caractères sont déjà colorés dans l'atlas
    return textureColor;
}

// Version alternative avec couleur uniforme (optionnelle)
fragment float4 textFS_Uniform(
    TextVertexOut in [[stage_in]],
    texture2d<float> fontTexture [[texture(0)]],
    constant float4& textColor [[buffer(0)]])
{
    constexpr sampler texSampler(
        mag_filter::linear,
        min_filter::linear,
        address::clamp_to_edge
    );
    
    float4 textureColor = fontTexture.sample(texSampler, in.texCoord);
    
    // Utilise l'alpha de la texture mais applique une couleur uniforme
    return float4(textColor.rgb, textureColor.a * textColor.a);
}
