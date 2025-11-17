//
//  JDLV.metal
//  Episan
//
//  Created by Rémy on 17/11/2025.
//

#include <metal_stdlib>
using namespace metal;

#include "RMDLMainRenderer_shared.h"

kernel void JDLVCompute(device const uint* sourceGrid [[buffer(BufferIndexMeshPositions)]],
                        device uint* destGrid [[buffer(BufferIndexMeshGenerics)]],
                        constant JDLVState& gameState [[buffer(BufferIndexFrameData)]],
                        uint2 gid [[thread_position_in_grid]])
{
    if (gid.x >= gameState.width || gid.y >= gameState.height)
        return ;

    uint32_t index = gid.y * gameState.width + gid.x;

    uint32_t liveNeighbors = 0;
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0)
                continue;

            uint32_t nx = gid.x + dx;
            uint32_t ny = gid.y + dy;

            if (nx >= 0 && nx < gameState.width && ny >= 0 && ny < gameState.height)
            {
                uint32_t neighborIndex = ny * gameState.width + nx;
                if (sourceGrid[neighborIndex] > 0)
                    liveNeighbors++;
            }
        }
    }

    uint currentState = sourceGrid[index];
    uint newState = 0;

    if (currentState == 1)
    {
        if (liveNeighbors == 2 || liveNeighbors == 3)
            newState = 1;
        else if (liveNeighbors == 3)
            newState = 1;
    }

    destGrid[index] = newState;
}

struct VertexOut
{
    float4 position [[position]];
    float2 texCoord;
};

vertex VertexOut JDLVVertex(constant uint* grid [[buffer(BufferIndexMeshPositions)]],
                            constant JDLVState& gameState [[buffer(BufferIndexMeshGenerics)]],
                            uint vertexID [[vertex_id]])
{
    VertexOut out;

    float2 positions[6] =
    {
        float2(-1.0, -1.0), float2(1.0, -1.0), float2(-1.0, 1.0),
        float2(-1.0, 1.0), float2(1.0, -1.0), float2(1.0, 1.0)
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
                             constant uint* grid [[buffer(BufferIndexMeshPositions)]],
                             constant JDLVState& gameState [[buffer(BufferIndexMeshGenerics)]])
{
    uint x = uint(in.texCoord.x * gameState.width);
    uint y = uint((1.0 - in.texCoord.y) * gameState.height);
    uint index = y * gameState.width + x;

    uint cellState = grid[index];

    float4 color = cellState > 0 ? float4(1.0, 1.0, 1.0, 1.0) : float4(0.0, 0.0, 0.0, 1.0);

    return color;
}
