/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLGameCoordinator.cpp            +++    +++ **/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 27/10/2025 18:44:15      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define MTLFX_PRIVATE_IMPLEMENTATION

#include <Foundation/Foundation.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <Metal/Metal.hpp>
#include <MetalFX/MetalFX.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>
#include <utility>
#include <variant>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <thread>
#include <sys/sysctl.h>
#include <stdlib.h>

#include "RMDLGameCoordinator.hpp"

#define NUM_ELEMS(arr) (sizeof(arr) / sizeof(arr[0]))

const int GameCoordinator::kMaxFramesInFlight = 3;
static constexpr size_t kInstanceRows = 10;
static constexpr size_t kNumInstances = 30;
static constexpr uint32_t kTextureWidth = 128;
static constexpr uint32_t kTextureHeight = 128;
static constexpr uint64_t kPerFrameBumpAllocatorCapacity = 1024;

struct Uniforms
{
    simd::float4x4 modelViewProjectionMatrix;
    simd::float4x4 modelMatrix;
};

static const float cubeVertices[] = {
    // positions          // colors
    -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 0.0f
};

static const uint16_t cubeIndices[] = {
    0, 1, 2, 2, 3, 0, // front
    4, 5, 6, 6, 7, 4, // back
    0, 4, 7, 7, 3, 0, // left
    1, 5, 6, 6, 2, 1, // right
    3, 2, 6, 6, 7, 3, // top
    0, 1, 5, 5, 4, 0  // bottom
};

static const uint16_t indices[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
};

namespace shader_types
{
    struct VertexData
    {
        simd::float3 position;
        simd::float3 normal;
        simd::float2 texcoord;
    };

    struct InstanceData
    {
        simd::float4x4 instanceTransform;
        //simd::float3x3 instanceNormalTransform;
        simd::float4 instanceColor;
    };

    struct CameraData
    {
        simd::float4x4 perspectiveTransform;
        simd::float4x4 worldTransform;
        simd::float3x3 worldNormalTransform;
    };
}

GameCoordinator::GameCoordinator(MTL::Device* pDevice,
                                 MTL::PixelFormat layerPixelFormat,
                                 NS::UInteger width,
                                 NS::UInteger height,
                                 NS::UInteger gameUICanvasSize,
                                 const std::string& assetSearchPath)
    : _layerPixelFormat(layerPixelFormat)
    , _pDevice(pDevice->retain())
    , _frame(0)
    , _maxEDRValue(1.0f)
    , _brightness(500)
    , _edrBias(0)
    , _highScore(0)
    , _prevScore(0)
    , _angle(0.f)
    , _animationIndex(0)
    //, _pCubeVertexBuffer(nullptr)
{
    printf("GameCoordinator constructor called\n");
    unsigned int numThreads = std::thread::hardware_concurrency();
    std::cout << "number of Threads = std::thread::hardware_concurrency : " << numThreads << std::endl;
    _pCommandQueue = _pDevice->newCommandQueue();
    setupCamera();
    std::cout << sizeof(uint64_t) << std::endl;
//    buildRenderPipelines(assetSearchPath);
//    buildComputePipelines(assetSearchPath);
//    buildSamplers();
    const NS::UInteger nativeWidth = (NS::UInteger)(width / 1.2);
    const NS::UInteger nativeHeight = (NS::UInteger)(height / 1.2);


//    buildShaders();
//    buildComputePipeline();
//    buildDepthStencilStates();
//    buildTextures();
//    buildBuffers();
    //setupPipelineCamera();
    _semaphore = dispatch_semaphore_create( GameCoordinator::kMaxFramesInFlight );
    printf("GameCoordinator constructor completed\n");
}

GameCoordinator::~GameCoordinator()
{
}

void GameCoordinator::setupCamera()
{
}



void GameCoordinator::buildCubeBuffers()
{
}

void GameCoordinator::buildRenderPipelines( const std::string& shaderSearchPath )
{
}

void GameCoordinator::buildComputePipelines( const std::string& shaderSearchPath )
{
    // Build any compute pipelines
}

void GameCoordinator::buildRenderTextures(NS::UInteger nativeWidth, NS::UInteger nativeHeight,
                                          NS::UInteger presentWidth, NS::UInteger presentHeight)
{
}

void GameCoordinator::moveCamera( simd::float3 translation )
{
}

void GameCoordinator::rotateCamera(float deltaYaw, float deltaPitch)
{
}

void GameCoordinator::setCameraAspectRatio(float aspectRatio)
{
}

void GameCoordinator::draw( CA::MetalDrawable* pDrawable, double targetTimestamp )
{
    NS::AutoreleasePool *pPool = NS::AutoreleasePool::alloc()->init();
    pPool->release();
}
