#ifndef RMDLMESHUTILS_HPP
#define RMDLMESHUTILS_HPP

#include <Metal/Metal.hpp>

#include <vector>
#include <simd/simd.h>
#include <sstream>

#include "RMDLFontLoader.h"
#include "RMDLUtils.hpp"

struct IndexedMesh
{
    MTL::Buffer*   pVertices;
    MTL::Buffer*   pIndices;
    uint32_t       numIndices;
    MTL::IndexType indexType;
    MTL::Winding   winding;
};

namespace mesh_utils
{
    IndexedMesh newCubeMesh( float size, MTL::Device* pDevice );
    IndexedMesh newHorizontalQuad( float size, uint32_t divs, MTL::Device* pDevice );
    IndexedMesh newScreenQuad( MTL::Device* pDevice, float horizontalScale = 1.0f, float verticalScale = 1.0f );
    void        releaseMesh(IndexedMesh* pIndexedMesh);
    IndexedMesh newTextMesh( const std::string& text, const FontAtlas& fontAtlas, MTL::Device* pDevice );
}

#endif // RMDLMESHUTILS_HPP
