#ifndef RMDLFONTLOADER_H
#define RMDLFONTLOADER_H

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include <string>
#include <simd/simd.h>

#include "RMDLUtils.hpp"

MTL::Texture* newTextureFromFile( const std::string& texturePath, MTL::Device* pDevice );
MTL::Texture* newTextureArrayFromFiles( const std::vector<std::string>& texturePaths, MTL::Device* pDevice, MTL::CommandBuffer* pCommandBuffer );

static char g_chars[] = {
    '!', '\"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',',  '-', '.', '/', '0',
    '1', '2',  '3', '4', '5', '6', '7',  '8', '9', ':', ';', '<',  '=', '>', '?', '@',
    'A', 'B',  'C', 'D', 'E', 'F', 'G',  'H', 'I', 'J', 'K', 'L',  'M', 'N', 'O', 'P',
    'Q', 'R',  'S', 'T', 'U', 'V', 'W',  'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '`',
    'a', 'b',  'c', 'd', 'e', 'f', 'g',  'h', 'i', 'j', 'k', 'l',  'm', 'n', 'o', 'p',
    'q', 'r',  's', 't', 'u', 'v', 'w',  'x', 'y', 'z', '{', '|',  '}', '~'
};

constexpr size_t kNumCharacters = sizeof(g_chars) / sizeof(g_chars[0]);

struct FontAtlas
{
    struct CharUVs
    {
        simd::float2 nw;
        simd::float2 ne;
        simd::float2 se;
        simd::float2 sw;
    };
    
    NS::SharedPtr<MTL::Texture> texture;
    CharUVs charToUVs[kNumCharacters];
};

FontAtlas newFontAtlas( MTL::Device* pDevice );

struct FiraCode
{
    struct CharUVs
    {
        simd::float2 nw;
        simd::float2 ne;
        simd::float2 se;
        simd::float2 sw;
    };
    
    NS::SharedPtr<MTL::Texture> texture;
    CharUVs charToUVs[kNumCharacters];
};

FiraCode newFiraCode( MTL::Device* pDevice );

#endif // RMDLFONTLOADER_H
