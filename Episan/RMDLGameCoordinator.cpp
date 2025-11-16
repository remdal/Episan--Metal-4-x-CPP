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

#define kMaxFramesInFlight 3

#define NUM_ELEMS(arr) (sizeof(arr) / sizeof(arr[0]))

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

static MTL::Library* newLibraryFromBytecode( const std::vector<uint8_t>& bytecode, MTL::Device* pDevice )
{
    NS::Error* pError = nullptr;
    dispatch_data_t data = dispatch_data_create(bytecode.data(), bytecode.size(), dispatch_get_main_queue(), DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    MTL::Library* pLib = pDevice->newLibrary(data, &pError);
    if (!pLib)
    {
        printf("Error building Metal library: %s\n", pError->localizedDescription()->utf8String());
        assert(pLib);
    }
    CFRelease(data);
    return pLib;
}

GameCoordinator::GameCoordinator(MTL::Device* pDevice,
                                 MTL::PixelFormat layerPixelFormat,
                                 NS::UInteger width,
                                 NS::UInteger height,
                                 NS::UInteger gameUICanvasSize,
                                 const std::string& assetSearchPath)
    : _layerPixelFormat(layerPixelFormat)
    , _pCommandQueue(nullptr)
    , _pCommandBuffer(nullptr)
    , _pArgumentTable(nullptr)
    , _pResidencySet(nullptr)
    , _sharedEvent(nullptr)
    , _semaphore(nullptr)
    , _pDevice(pDevice->retain())
    , _pPSO(nullptr)
    , _pDepthStencilState(nullptr)
    , _pTexture(nullptr)
    , _uniformBufferIndex(0)
    , _currentFrameIndex(0)
    , _pShaderLibrary(nullptr)
    , _frameNumber(0)
    , _presentOrtho{}
    , _pUniformBuffer(nullptr)
    , _highScore(0)
    , _prevScore(0)
    , _maxEDRValue(1.0f)
    , _brightness(500)
    , _edrBias(0)
    , _pPacingEvent(nullptr)
    , _pacingTimeStampIndex(0)
    , _pPresentPipeline(nullptr)
    , _pInstancedSpritePipeline(nullptr)
    , _pSampler(nullptr)
    , _pMapPSO(nullptr)
    , _pCameraPSO(nullptr)
    , _pComputePSO(nullptr)
    , _pVertexDataBuffer(nullptr)
    , _pIndexBuffer(nullptr)
    , _pTextureAnimationBuffer(nullptr)
    , _angle(0.f)
    , _frameP(0)
    , _animationIndex(0)
    , _pVertexDataBufferMap(nullptr)
    , _pIndexBufferMap(nullptr)
{
    printf("GameCoordinator constructor called\n");

    unsigned int numThreads = std::thread::hardware_concurrency();
    std::cout << "std::thread::hardware_concurrency : " << numThreads << std::endl;

    std::cout << "size of uint64_t : "<< sizeof(uint64_t) << " bits" << std::endl;

    if (!_pDevice->supportsFamily(MTL::GPUFamily::GPUFamilyMetal4))
        std::cerr << "Metal features required by this app are not supported on this device (GPUFamily::GPUFamilyMetal4 check failed)." << std::endl;

#pragma mark init
    _pCommandQueue = _pDevice->newMTL4CommandQueue();
    _pCommandBuffer = _pDevice->newCommandBuffer();
    _pShaderLibrary = _pDevice->newDefaultLibrary();

    for (uint8_t i = 0; i < kMaxFramesInFlight; i++)
    {
        _pTriangleDataBuffer[i] = _pDevice->newBuffer( sizeof(TriangleData), MTL::ResourceStorageModeShared );
        std::string name = "_pTriangleDataBuffer[" + std::to_string(i) + "]";
        _pTriangleDataBuffer[i]->setLabel( NS::String::string( name.c_str(), NS::ASCIIStringEncoding ) );

        _pCommandAllocator[i] = _pDevice->newCommandAllocator();
    }

    makeArgumentTable();
    makeResidencySet();

    _pViewportSize.x = (float)width;
    _pViewportSize.y = (float)height;
    _pViewportSizeBuffer = _pDevice->newBuffer(sizeof(_pViewportSize), MTL::ResourceStorageModeShared);

    compileRenderPipeline( _layerPixelFormat, assetSearchPath );

    _sharedEvent = _pDevice->newSharedEvent();
    _sharedEvent->setSignaledValue(_currentFrameIndex); // Is correct? static_cast<uint64_t>

    _semaphore = dispatch_semaphore_create( kMaxFramesInFlight );

    setupCamera();
}

GameCoordinator::~GameCoordinator()
{
    for (uint8_t i = 0; i < kMaxFramesInFlight; ++i)
    {
        if (_pTriangleDataBuffer[i]) { _pTriangleDataBuffer[i]->release(); _pTriangleDataBuffer[i] = nullptr; }
        if (_pCommandAllocator[i])   { _pCommandAllocator[i]->release();   _pCommandAllocator[i]   = nullptr; }
        if (_pInstanceDataBuffer[i]) { _pInstanceDataBuffer[i]->release(); _pInstanceDataBuffer[i] = nullptr; }
    }

    if (_pUniformBuffer)          { _pUniformBuffer->release();          _pUniformBuffer = nullptr; }
    if (_pIndexBuffer)            { _pIndexBuffer->release();            _pIndexBuffer = nullptr; }
    if (_pVertexDataBuffer)       { _pVertexDataBuffer->release();       _pVertexDataBuffer = nullptr; }
    if (_pTextureAnimationBuffer) { _pTextureAnimationBuffer->release(); _pTextureAnimationBuffer = nullptr; }
    if (_pIndexBufferMap)         { _pIndexBufferMap->release();         _pIndexBufferMap = nullptr; }
    if (_pVertexDataBufferMap)    { _pVertexDataBufferMap->release();    _pVertexDataBufferMap = nullptr; }

    if (_pTexture)                { _pTexture->release();                _pTexture = nullptr; }
    if (_pSampler)                { _pSampler->release();                _pSampler = nullptr; }

    if (_pDepthStencilState)      { _pDepthStencilState->release();      _pDepthStencilState = nullptr; }
    if (_pPSO)                    { _pPSO->release();                    _pPSO = nullptr; }
    if (_pMapPSO)                 { _pMapPSO->release();                 _pMapPSO = nullptr; }
    if (_pCameraPSO)              { _pCameraPSO->release();              _pCameraPSO = nullptr; }
    if (_pComputePSO)             { _pComputePSO->release();             _pComputePSO = nullptr; }
    if (_pPresentPipeline)        { _pPresentPipeline->release();        _pPresentPipeline = nullptr; }

    _pBackbuffer.reset();
    _pUpscaledbuffer.reset();
    _pBackbufferAdapter.reset();
    _pUpscaledbufferAdapter.reset();
    _pUpscaledbufferAdapterP.reset();
    _pSpatialScaler.reset();
    _pPacingEvent.reset();

    if (_pShaderLibrary) { _pShaderLibrary->release(); _pShaderLibrary = nullptr; }
    if (_pCommandBuffer) { _pCommandBuffer->release(); _pCommandBuffer = nullptr; }
    if (_pCommandQueue)  { _pCommandQueue->release();  _pCommandQueue  = nullptr; }

    if (_pResidencySet)  { _pResidencySet->release();  _pResidencySet  = nullptr; }
    if (_pArgumentTable) { _pArgumentTable->release(); _pArgumentTable = nullptr; }
    if (_sharedEvent)    { _sharedEvent->release();    _sharedEvent    = nullptr; }

    _textureAssets.clear();

    if (_semaphore)
    {
        dispatch_release(_semaphore);
        _semaphore = nullptr;
    }

    if (_pDevice) { _pDevice->release(); _pDevice = nullptr; }
}

void GameCoordinator::makeArgumentTable()
{
    NS::Error* pError = nullptr;

    MTL4::ArgumentTableDescriptor* argumentTableDescriptor = MTL4::ArgumentTableDescriptor::alloc()->init();
    argumentTableDescriptor->setMaxBufferBindCount(2);
    argumentTableDescriptor->setLabel( NS::String::string( "p argument table descriptor triangle", NS::ASCIIStringEncoding ) );

    _pArgumentTable = _pDevice->newArgumentTable(argumentTableDescriptor, &pError);
    argumentTableDescriptor->release();
}

void GameCoordinator::makeResidencySet()
{
    NS::Error* pError = nullptr;

    MTL::ResidencySetDescriptor* residencySetDescriptor = MTL::ResidencySetDescriptor::alloc()->init();
    residencySetDescriptor->setLabel( NS::String::string( "p residency set", NS::ASCIIStringEncoding ) );

    _pResidencySet = _pDevice->newResidencySet(residencySetDescriptor, &pError);
    _pResidencySet->requestResidency();
    _pCommandQueue->addResidencySet(_pResidencySet); // .get() is for struct

    for (uint8_t i = 0u; i < kMaxFramesInFlight; ++i)
    {
        _pResidencySet->addAllocation(_pTriangleDataBuffer[i]);
    }
    _pResidencySet->addAllocation(_pViewportSizeBuffer);

    _pResidencySet->commit();

    _pCommandQueue->addResidencySet(_pResidencySet);
//    _pCommandQueue->addResidencySet((CA::MetalLayer *)layer)
    
    residencySetDescriptor->release();
}

void GameCoordinator::compileRenderPipeline( MTL::PixelFormat _layerPixelFormat, const std::string& assetSearchPath )
{
    NS::Error* pError= nullptr;

    MTL4::Compiler* compiler = _pDevice->newCompiler( MTL4::CompilerDescriptor::alloc()->init(), &pError );

    MTL4::RenderPipelineDescriptor* descriptor = MTL4::RenderPipelineDescriptor::alloc()->init();
    descriptor->setLabel( NS::String::string( "Metal 4 render pipeline", NS::ASCIIStringEncoding ) );
    descriptor->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormatRGBA16Float );

    MTL4::LibraryFunctionDescriptor* vertexFunction = MTL4::LibraryFunctionDescriptor::alloc()->init();
    vertexFunction->setName(MTLSTR("vertexShaderTriangle"));
    vertexFunction->setLibrary(_pShaderLibrary);

    MTL4::LibraryFunctionDescriptor* fragmentFunction = MTL4::LibraryFunctionDescriptor::alloc()->init();
    fragmentFunction->setName( NS::String::string( "fragmentShaderTriangle", NS::ASCIIStringEncoding ) );
    fragmentFunction->setLibrary(_pShaderLibrary);

    descriptor->setVertexFunctionDescriptor(vertexFunction);
    descriptor->setFragmentFunctionDescriptor(fragmentFunction);

//    NS::URL* url = NS::URL::fileURLWithPath( NS::String::string( assetSearchPath.c_str(), NS::ASCIIStringEncoding ) );
//    MTL4::Archive* archivesArray = _pDevice->newArchive(url, &pError);
    MTL4::CompilerTaskOptions* compilerTaskOptions = MTL4::CompilerTaskOptions::alloc()->init();
    //compilerTaskOptions->setLookupArchives(archivesArray);

    _pPSO = compiler->newRenderPipelineState(descriptor, nullptr, &pError);
    compilerTaskOptions->release();
    fragmentFunction->release();
    vertexFunction->release();
    descriptor->release();
    compiler->release();
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
    NS::Error* pError = nullptr;
    NS::AutoreleasePool *pPool = NS::AutoreleasePool::alloc()->init();

    _currentFrameIndex += 1;
    const uint32_t frameIndex = _currentFrameIndex % kMaxFramesInFlight;
    std::string label = "Frame: " + std::to_string(_currentFrameIndex);
    
    if (_currentFrameIndex > kMaxFramesInFlight)
    {
        uint64_t const timeStampToWait = _currentFrameIndex - kMaxFramesInFlight;
        _pPacingEvent->waitUntilSignaledValue(timeStampToWait, DISPATCH_TIME_FOREVER);
    }

    MTL4::CommandAllocator* pFrameAllocator = _pCommandAllocator[_frameNumber];
    pFrameAllocator->reset();

    _pCommandBuffer->beginCommandBuffer(pFrameAllocator);
    _pCommandBuffer->setLabel( NS::String::string( label.c_str(), NS::ASCIIStringEncoding ) );

    MTL4::RenderCommandEncoder* pRenderPassEncoder;
    MTL4::RenderPassDescriptor* pRenderPassDescriptor = MTL4::RenderPassDescriptor::alloc()->init();
    
//    MTL::CommandBufferDescriptor* pCommandBufferDescription = MTL::CommandBufferDescriptor::alloc()->init();
//    _pCommandBuffer = _pCommandQueue->_pDevice->newCommandBuffer(pCommandBufferDescription, &pError);
//    
//    

    MTL::RenderPassColorAttachmentDescriptor* color0 = pRenderPassDescriptor->colorAttachments()->object(0);
    color0->setTexture(pDrawable->texture());
    color0->setLoadAction( MTL::LoadActionClear );
    color0->setStoreAction( MTL::StoreActionStore );
    color0->setClearColor( MTL::ClearColor(0.1, 0.1, 0.1, 1.0) );
//
//    // Encode a minimal render pass (no draw calls yet)
//    MTL4::RenderCommandEncoder* pRenderCommandEncoder = _pCommandBuffer->renderCommandEncoder(pRenderPassDescriptor);
//    pRenderCommandEncoder->setRenderPipelineState(_pPSO);
//    pRenderCommandEncoder->endEncoding();
//
//    // Present and commit
//    _pCommandBuffer->presentDrawable(pDrawable);
//    _pCommandBuffer->commit();
//    id<CAMetalDrawable> currentDrawable = view.currentDrawable;
//    // Instruct the queue to wait until the drawable is ready to receive output from the render pass.
//    [commandQueue waitForDrawable:currentDrawable];
//    // Run the command buffer on the GPU by submitting it the Metal device's queue.
//    [commandQueue commit:&commandBuffer count:1];
//    // Notify the drawable that the GPU is done running the render pass.
//    [commandQueue signalDrawable:currentDrawable];
//    // Instruct the drawable to show itself on the device's display when the render pass completes.
//    [currentDrawable present];

//x    pCommandBufferDescription->release();
    pPool->release();
}
