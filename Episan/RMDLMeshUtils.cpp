#include <simd/simd.h>
#include <sstream>

#include "RMDLUtils.hpp"
#include "RMDLMeshUtils.hpp"

struct VertexDataWithNormal
{
    simd::float4 position;
    simd::float4 normal;
    simd::float4 texcoord;
};

IndexedMesh mesh_utils::newCubeMesh( float size, MTL::Device* pDevice )
{
    const float s = size * 0.5f;

    VertexDataWithNormal verts[] = {
        //                                                      Texture
        //   Positions                   Normals               Coordinates
        { { -s, -s, +s, 1.f }, { 0.f, 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f, 0.f } },
        { { +s, -s, +s, 1.f }, { 0.f, 0.f, 1.f, 0.f }, { 1.f, 1.f, 0.f, 0.f } },
        { { +s, +s, +s, 1.f }, { 0.f, 0.f, 1.f, 0.f }, { 1.f, 0.f, 0.f, 0.f } },
        { { -s, +s, +s, 1.f }, { 0.f, 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f, 0.f } },

        { { +s, -s, +s, 1.f }, { 1.f, 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f, 0.f } },
        { { +s, -s, -s, 1.f }, { 1.f, 0.f, 0.f, 0.f }, { 1.f, 1.f, 0.f, 0.f } },
        { { +s, +s, -s, 1.f }, { 1.f, 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 0.f } },
        { { +s, +s, +s, 1.f }, { 1.f, 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, 0.f } },

        { { +s, -s, -s, 1.f }, { 0.f, 0.f, -1.f, 0.f }, { 0.f, 1.f, 0.f, 0.f } },
        { { -s, -s, -s, 1.f }, { 0.f, 0.f, -1.f, 0.f }, { 1.f, 1.f, 0.f, 0.f } },
        { { -s, +s, -s, 1.f }, { 0.f, 0.f, -1.f, 0.f }, { 1.f, 0.f, 0.f, 0.f } },
        { { +s, +s, -s, 1.f }, { 0.f, 0.f, -1.f, 0.f }, { 0.f, 0.f, 0.f, 0.f } },

        { { -s, -s, -s, 1.f }, { -1.f, 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f, 0.f } },
        { { -s, -s, +s, 1.f }, { -1.f, 0.f, 0.f, 0.f }, { 1.f, 1.f, 0.f, 0.f } },
        { { -s, +s, +s, 1.f }, { -1.f, 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 0.f } },
        { { -s, +s, -s, 1.f }, { -1.f, 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, 0.f } },

        { { -s, +s, +s, 1.f }, { 0.f, 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f, 0.f } },
        { { +s, +s, +s, 1.f }, { 0.f, 1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f, 0.f } },
        { { +s, +s, -s, 1.f }, { 0.f, 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 0.f } },
        { { -s, +s, -s, 1.f }, { 0.f, 1.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, 0.f } },

        { { -s, -s, -s, 1.f }, { 0.f, -1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f, 0.f } },
        { { +s, -s, -s, 1.f }, { 0.f, -1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f, 0.f } },
        { { +s, -s, +s, 1.f }, { 0.f, -1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 0.f } },
        { { -s, -s, +s, 1.f }, { 0.f, -1.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, 0.f } }
    };

    uint16_t indices[] = {
         0,  1,  2,  2,  3,  0, /* front */
         4,  5,  6,  6,  7,  4, /* right */
         8,  9, 10, 10, 11,  8, /* back */
        12, 13, 14, 14, 15, 12, /* left */
        16, 17, 18, 18, 19, 16, /* top */
        20, 21, 22, 22, 23, 20, /* bottom */
    };

    const size_t vertexDataSize = sizeof(verts);
    const size_t indexDataSize  = sizeof(indices);

    MTL::Buffer* pVertexBuffer = pDevice->newBuffer( vertexDataSize, MTL::ResourceStorageModeShared );
    MTL::Buffer* pIndexBuffer  = pDevice->newBuffer( indexDataSize, MTL::ResourceStorageModeShared );

    ft_memcpy(pVertexBuffer->contents(), verts, vertexDataSize);
    ft_memcpy(pIndexBuffer->contents(), indices, indexDataSize);

    IndexedMesh cubeMesh
    {
        .pVertices  = pVertexBuffer,
        .pIndices   = pIndexBuffer,
        .numIndices = sizeof(indices) / sizeof(indices[0]),
        .indexType  = MTL::IndexTypeUInt16,
        .winding    = MTL::WindingCounterClockwise
    };

    return (cubeMesh);
}

IndexedMesh mesh_utils::newHorizontalQuad( float size, uint32_t divs, MTL::Device* pDevice )
{

    const float s = size * 0.5f;

    simd::float4 floorVertexData[] =
    {
        { -s, -0.5f, +s - 10, 1.f },
        { +s, -0.5f, +s - 10, 1.f },
        { +s, -0.5f, -s - 10, 1.f },
        { -s, -0.5f, -s - 10, 1.f }
    };

    std::vector<simd::float4> tessellatedFloorVertices;
    for (uint32_t z = 0; z <= divs; ++z)
    {
        for (uint32_t x = 0; x <= divs; ++x)
        {
            float dx = x / (float)divs;
            float dz = z / (float)divs;

            float px = simd::lerp(floorVertexData[0].x, floorVertexData[1].x, dx);
            float pz = simd::lerp(floorVertexData[0].z, floorVertexData[2].z, dz);
            tessellatedFloorVertices.emplace_back(simd::float4 { px, floorVertexData[0].y, pz, 1.f });
        }
    }

    std::vector<uint32_t> tessellatedFloorIndices;
    uint32_t indx = 0;
    for (uint32_t i = 0; i < divs * divs; ++i)
    {
        uint32_t i0 = indx;
        uint32_t i1 = indx + (divs + 1);
        uint32_t i2 = indx + (divs + 1) + 1;

        uint32_t i3 = indx;
        uint32_t i4 = indx + (divs + 1) + 1;
        uint32_t i5 = indx + 1;

        tessellatedFloorIndices.emplace_back(i0);
        tessellatedFloorIndices.emplace_back(i1);
        tessellatedFloorIndices.emplace_back(i2);

        tessellatedFloorIndices.emplace_back(i3);
        tessellatedFloorIndices.emplace_back(i4);
        tessellatedFloorIndices.emplace_back(i5);

        if ((tessellatedFloorIndices.size() % (6 * divs)) != 0)
        {
            ++indx;
        }
        else
        {
            indx += 2;
        }
    }

    IndexedMesh horizontalQuad;

    horizontalQuad.winding    = MTL::WindingClockwise;
    horizontalQuad.indexType  = MTL::IndexTypeUInt32;
    horizontalQuad.numIndices = (uint32_t)tessellatedFloorIndices.size();

    size_t len               = sizeof(simd::float4) * tessellatedFloorVertices.size();
    horizontalQuad.pVertices = pDevice->newBuffer(len, MTL::ResourceStorageModeShared);
    ft_memcpy(horizontalQuad.pVertices->contents(), tessellatedFloorVertices.data(), len);

    len                     = sizeof(uint32_t) * tessellatedFloorIndices.size();
    horizontalQuad.pIndices = pDevice->newBuffer(len, MTL::ResourceStorageModeShared);
    ft_memcpy(horizontalQuad.pIndices->contents(), tessellatedFloorIndices.data(), len);

    return (horizontalQuad);
}

struct VertexData
{
    simd::float4 position;
    simd::float4 texcoord;
};

static_assert(sizeof(VertexData) == 32);

IndexedMesh mesh_utils::newScreenQuad(MTL::Device* pDevice, float horizontalScale, float verticalScale)
{
    const float hs = horizontalScale/2;
    const float vs = verticalScale/2;
    
    std::vector<VertexData> vertexData
    {
        VertexData {
            .position = simd::float4{-1.f*hs, 1.f*vs, 0.0, 1.0},
            .texcoord = simd::float4{0.0, 0.0}
        },
        VertexData {
            .position = simd::float4{-1.f*hs, -1.f*vs, 0.0, 1.0},
            .texcoord = simd::float4{0.0, 1.0}
        },
        VertexData {
            .position = simd::float4{+1.f*hs, -1.f*vs, 0.0, 1.0},
            .texcoord = simd::float4{1.0, 1.0}
        },
        VertexData {
            .position = simd::float4{+1.f*hs, +1.f*vs, 0.0, 1.0},
            .texcoord = simd::float4{1.0, 0.0}
        }
    };
    
    std::vector<uint16_t> indexData
    {
        0, 1, 2,
        2, 3, 0
    };
    
    MTL::Buffer* pBuffer = pDevice->newBuffer( vertexData.size() * sizeof(VertexData), MTL::ResourceStorageModeShared );
    memcpy(pBuffer->contents(), vertexData.data(), vertexData.size() * sizeof(VertexData));
    
    MTL::Buffer* pIndexBuffer = pDevice->newBuffer( indexData.size() * sizeof(uint16_t), MTL::ResourceStorageModeShared );
    memcpy(pIndexBuffer->contents(), indexData.data(), indexData.size() * sizeof(uint16_t));
    
    return IndexedMesh
    {
        .pVertices = pBuffer,
        .pIndices = pIndexBuffer,
        .numIndices = (uint32_t)indexData.size(),
        .indexType = MTL::IndexTypeUInt16,
        .winding = MTL::WindingCounterClockwise
    };
}

void mesh_utils::releaseMesh(IndexedMesh* pIndexedMesh)
{
    pIndexedMesh->pVertices->release();
    pIndexedMesh->pIndices->release();
    pIndexedMesh->pVertices = nullptr;
    pIndexedMesh->pIndices = nullptr;
}

IndexedMesh mesh_utils::newTextMesh( const std::string& text, const FontAtlas& fontAtlas, MTL::Device* pDevice )
{
    const size_t numVertices = 4 * text.size();
    const float charWidth = 1.0f;
    const float meshWidth = charWidth * (float)text.size();
    std::vector<VertexData> meshVertices(numVertices);
    std::vector<uint16_t> indices;
    for (size_t i = 0; i < text.size(); ++i)
    {
        float x = i / (float)(text.size() - 1);
        float tx = (meshWidth * x) - (meshWidth * 0.5f);
        FontAtlas::CharUVs UVMap = {0};
        if (text[i] >= '!' && text[i] < '_')
        {
            UVMap = (fontAtlas.charToUVs[text[i]-'!']);
        }
        
        meshVertices[i * 4 + 0].position = simd_make_float4(-0.5 + tx, +0.5, 0.0, 1.0);
        meshVertices[i * 4 + 0].texcoord = simd_make_float4(UVMap.nw, 0, 1);

        meshVertices[i * 4 + 1].position = simd_make_float4(-0.5 + tx, -0.5, 0.0, 1.0);
        meshVertices[i * 4 + 1].texcoord = simd_make_float4(UVMap.sw, 0, 1);

        meshVertices[i * 4 + 2].position = simd_make_float4(+0.5 + tx, -0.5, 0.0, 1.0);
        meshVertices[i * 4 + 2].texcoord = simd_make_float4(UVMap.se, 0, 1);
        
        meshVertices[i * 4 + 3].position = simd_make_float4(+0.5 + tx, +0.5, 0.0, 1.0);
        meshVertices[i * 4 + 3].texcoord = simd_make_float4(UVMap.ne, 0, 1);
        

        indices.push_back(i * 4 + 0 );
        indices.push_back(i * 4 + 1);
        indices.push_back(i * 4 + 2);
        
        indices.push_back(i * 4 + 2);
        indices.push_back(i * 4 + 3);
        indices.push_back(i * 4 + 0);
    }
    IndexedMesh result;
    result.pVertices = pDevice->newBuffer(meshVertices.data(), sizeof(VertexData) * meshVertices.size(), MTL::ResourceStorageModeShared);
    result.pIndices = pDevice->newBuffer(indices.data(), sizeof(uint16_t) * indices.size(), MTL::ResourceStorageModeShared);
    result.numIndices = (uint32_t)indices.size();
    result.indexType = MTL::IndexTypeUInt16;
    result.winding = MTL::WindingCounterClockwise;
    {
        std::stringstream ss;
        ss << "TextMesh: " << text << "(vertices)";
        result.pVertices->setLabel(NS::String::string(ss.str().c_str(), NS::UTF8StringEncoding));
    }
    {
        std::stringstream ss;
        ss << "TextMesh" << text << "(indices)";
        result.pIndices->setLabel(NS::String::string(ss.str().c_str(), NS::UTF8StringEncoding));
    }
    
    return (result);
}
