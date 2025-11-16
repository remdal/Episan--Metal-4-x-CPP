/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLObjLoader.hpp            +++     +++		**/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 21/09/2025 14:42:37      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef RMDLOBJLOADER_HPP
# define RMDLOBJLOADER_HPP

#import <Foundation/Foundation.hpp>
#import <Metal/Metal.hpp>
#import <MetalKit/MetalKit.hpp>

# import <simd/simd.h>
# import <vector>
# import <unordered_map>

# import "RMDLMainRenderer_shared.h"

#include <iostream>
#include <stdio.h>

template<> struct std::hash<RMDLObjVertex>
{
    std::size_t operator()(const RMDLObjVertex& k) const
    {
        std::size_t hash = 0;
        for (uint w = 0; w < sizeof(RMDLObjVertex) / sizeof(std::size_t); w++)
            hash ^= (((std::size_t*)&k)[w] ^ (hash << 8) ^ (hash >> 8));
        std::cout << hash << '\n';
        return (hash);
    }
};

class RMDLObjMesh
{
public:
    RMDLObjMesh();
    ~RMDLObjMesh();

    int indexCount();
    int vertexCount();
private:
    float           _boundingRadius;
    MTL::Buffer*    _vertexBuffer;
    MTL::Buffer*    _indexBuffer;
};

class RMDLObjLoader
{
public:
    RMDLObjLoader();
    ~RMDLObjLoader();

    RMDLObjLoader& initWithDevice( MTL::Device* device );

    RMDLObjMesh*    loadFromURL( NS::URL *inURL );
private:
    std::vector<simd::float3>                   _positions;
    std::vector<simd::float3>                   _normals;
    std::vector<simd::float3>                   _colors;
    float                                       _boundingSphereRadius;

    std::unordered_map<RMDLObjVertex, uint32_t> _vertexMap;

    MTL::Device*                                _device;
    std::vector<RMDLObjVertex>                  _vertices;
    std::vector<uint16_t>                       _indices;
};

#endif // !RMDLOBJLOADER_HPP
