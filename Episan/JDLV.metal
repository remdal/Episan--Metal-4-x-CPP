//
//  JDLV.metal
//  Episan
//
//  Created by Rémy on 17/11/2025.
//

#include <metal_stdlib>
using namespace metal;

#include "RMDLMainRenderer_shared.h"

kernel void JDLVCompute(device const uint* sourceGrid [[buffer(0)]],
                        device uint* destGrid [[buffer(1)]],
                        constant JDLVState& gameState [[buffer(2)]],
                        uint2 gid [[thread_position_in_grid]])
{
    if (gid.x >= gameState.width || gid.y >= gameState.height)
        return;

    uint index = gid.y * gameState.width + gid.x;

    uint liveNeighbors = 0;

    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0)
                continue;

            int sx = int(gid.x) + dx;
            int sy = int(gid.y) + dy;

            if (sx >= 0 && sy >= 0 && sx < int(gameState.width) && sy < int(gameState.height))
            {
                uint nx = uint(sx);
                uint ny = uint(sy);
                uint neighborIndex = ny * gameState.width + nx;
                if (sourceGrid[neighborIndex] > 0)
                {
                    liveNeighbors += 1;
                }
            }
        }
    }

    uint currentState = sourceGrid[index];
    uint newState = 0;

    if (currentState == 1)
    {
        if (liveNeighbors == 2 || liveNeighbors == 3)
            newState = 1;
        else
            newState = 0;
    }
    else
    {
        if (liveNeighbors == 3)
            newState = 1;
    }

    destGrid[index] = newState;
}

struct VertexOut
{
    float4 position [[position]];
    float2 texCoord;
};

vertex VertexOut JDLVVertex(constant uint* grid [[buffer(0)]],
                            constant JDLVState& gameState [[buffer(1)]],
                            uint vertexID [[vertex_id]])
{
    VertexOut out;

    float2 positions[6] =
    {
        float2(-1.0, -1.0), float2( 1.0, -1.0), float2(-1.0,  1.0),
        float2(-1.0,  1.0), float2( 1.0, -1.0), float2( 1.0,  1.0)
    };

    float2 texCoords[6] =
    {
        float2(0.0, 1.0), float2(1.0, 1.0), float2(0.0, 0.0),
        float2(0.0, 0.0), float2(1.0, 1.0), float2(1.0, 0.0)
    };

    out.position = float4(positions[vertexID], 0.0, 1.0);
    out.texCoord = texCoords[vertexID];

    return out;
}

fragment float4 JDLVFragment(VertexOut in [[stage_in]],
                             constant uint* grid [[buffer(0)]],
                             constant JDLVState& gameState [[buffer(1)]])
{
    float fx = clamp(in.texCoord.x, 0.0f, 0.99999994f); // slightly less than 1.0f
    float fy = clamp(in.texCoord.y, 0.0f, 0.99999994f);

    uint x = uint(fx * float(gameState.width));

    uint y = uint((1.0f - fy) * float(gameState.height));

    x = min(x, gameState.width  - 1);
    y = min(y, gameState.height - 1);

    uint index = y * gameState.width + x;

    uint cellState = grid[index];

    float4 color = (cellState > 0) ? float4(1.0, 1.0, 1.0, 1.0)
                                   : float4(0.0, 0.0, 0.0, 1.0);

    return color;
}
