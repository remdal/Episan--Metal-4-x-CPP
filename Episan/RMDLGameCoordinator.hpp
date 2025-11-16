/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLGameCoordinator.hpp           +++     +++	**/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 27/10/2025 15:45:19      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef RMDLGAMECOORDINATOR_HPP
#define RMDLGAMECOORDINATOR_HPP

#include <Foundation/Foundation.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <Metal/Metal.hpp>
#include <MetalFX/MetalFX.hpp>

#include <string>
#include <unordered_map>

#include "RMDLMainRenderer_shared.h"
#include "RMDLUtils.hpp"

#define kMaxBuffersInFlight 3
static const uint32_t NumLights = 256;

typedef struct TriangleData
{
    VertexData vertex0;
    VertexData vertex1;
    VertexData vertex2;
} TriangleData;

class GameCoordinator
{
public:
    GameCoordinator(MTL::Device* pDevice,
                    MTL::PixelFormat layerPixelFormat,
                    NS::UInteger width,
                    NS::UInteger height,
                    NS::UInteger gameUICanvasSize,
                    const std::string& assetSearchPath);
    ~GameCoordinator();

    void buildRenderPipelines( const std::string& shaderSearchPath );
    void buildComputePipelines( const std::string& shaderSearchPath );
    void buildRenderTextures(NS::UInteger nativeWidth, NS::UInteger nativeHeight,
                             NS::UInteger presentWidth, NS::UInteger presentHeight);
    void loadGameTextures( const std::string& textureSearchPath );
    //void loadGameSounds( const std::string& assetSearchPath, PhaseAudio* pAudioEngine );
    void buildSamplers();
    void buildMetalFXUpscaler(NS::UInteger inputWidth, NS::UInteger inputHeight,
                              NS::UInteger outputWidth, NS::UInteger outputHeight);

    void presentTexture( MTL::RenderCommandEncoder* pRenderEnc, MTL::Texture* pTexture );

    void resizeDrawable(float width, float height);
    void setMaxEDRValue(float value)     { _maxEDRValue = value; }
    void setBrightness(float brightness) { _brightness = brightness; }
    void setEDRBias(float edrBias)       { _edrBias = edrBias; }

    enum class HighScoreSource {
        Local,
        Cloud
    };
    
    void setHighScore(int highScore, HighScoreSource scoreSource);
    int highScore() const                { return _highScore; }

    void setupCamera();
    void moveCamera( simd::float3 translation );
    void rotateCamera(float deltaYaw, float deltaPitch);
    void setCameraAspectRatio(float aspectRatio);
    
    float _rotationAngle;
    void buildCubeBuffers();
    
    /* P */
    void buildShaders();
    void buildComputePipeline();
    void buildDepthStencilStates();
    void buildTextures();
    void buildBuffers();
    void generateMandelbrotTexture( MTL::CommandBuffer* pCommandBuffer );
    void draw( CA::MetalDrawable* pDrawable, double targetTimestamp );
    void buildShadersMap();
    void buildBuffersMap();
    
    void setupPipelineCamera();
    
    //MTL::Buffer** buildTriangleDataBuffer(NS::UInteger count);
    void makeArgumentTable();
    void makeResidencySet();
    void compileRenderPipeline( MTL::PixelFormat, const std::string& );

private:

    MTL::PixelFormat                    _layerPixelFormat;
    MTL4::CommandQueue*                 _pCommandQueue;
    MTL4::CommandBuffer*                _pCommandBuffer;
    MTL4::CommandAllocator*             _pCommandAllocator[kMaxBuffersInFlight];
    MTL4::ArgumentTable*                _pArgumentTable;
    MTL::ResidencySet*                  _pResidencySet;
    MTL::SharedEvent*                   _sharedEvent;
    dispatch_semaphore_t                _semaphore;
    MTL::Buffer*                        _pInstanceDataBuffer[kMaxBuffersInFlight];
    MTL::Buffer*                        _pTriangleDataBuffer[kMaxBuffersInFlight];
    MTL::Buffer*                        _pViewportSizeBuffer;
    MTL::Device*                        _pDevice;
    MTL::RenderPipelineState*           _pPSO;
    MTL::DepthStencilState*             _pDepthStencilState;
    MTL::Texture*                       _pTexture;
    uint8_t                             _uniformBufferIndex;
    uint64_t                            _currentFrameIndex;
    simd_uint2                          _pViewportSize;
    MTL::Library*                       _pShaderLibrary;
    int                                 _frameNumber;
    
    simd::float4x4 _presentOrtho;
    
    
    MTL::Buffer*                        _pUniformBuffer;
    
    NS::SharedPtr<MTL::Texture>         _pBackbuffer;
    NS::SharedPtr<MTL::Texture>         _pUpscaledbuffer;
    NS::SharedPtr<MTL::Texture>         _pBackbufferAdapter;
    NS::SharedPtr<MTL::Texture>         _pUpscaledbufferAdapter;


    int            _highScore;
    int            _prevScore;
    float          _maxEDRValue;
    float          _brightness;
    float          _edrBias;
    
    NS::SharedPtr<MTL::SharedEvent>     _pPacingEvent;
    uint64_t                            _pacingTimeStampIndex;

    MTL::RenderPipelineState*           _pPresentPipeline;
    MTL::RenderPipelineState*           _pInstancedSpritePipeline;
    NS::SharedPtr<MTLFX::SpatialScaler> _pSpatialScaler;

    // Assets:
    MTL::SamplerState*          _pSampler;

    std::unordered_map<std::string, NS::SharedPtr<MTL::Texture>> _textureAssets;

    /* P */

    
    MTL::RenderPipelineState*           _pMapPSO;
    MTL::RenderPipelineState*           _pCameraPSO;
    MTL::ComputePipelineState*          _pComputePSO;
    
    
    MTL::Buffer*                        _pVertexDataBuffer;
    //
    //MTL::Buffer*                _pCameraDataBuffer[kMaxFramesInFlight];
    MTL::Buffer*                _pIndexBuffer;
    MTL::Buffer*                _pTextureAnimationBuffer;
    float                       _angle;
    int                         _frameP;
    
    uint                        _animationIndex;
    NS::SharedPtr<MTL::Texture>         _pUpscaledbufferAdapterP;
    MTL::Buffer* _pVertexDataBufferMap;
    MTL::Buffer* _pIndexBufferMap;
    static const int            kMaxFramesInFlight;
};

#endif /* RMDLGAMECOORDINATOR_HPP */
