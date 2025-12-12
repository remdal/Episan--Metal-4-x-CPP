//
//  RMDLUI.hpp
//  Episan
//
//  Created by Rémy on 12/12/2025.
//

#ifndef RMDLUI_hpp
#define RMDLUI_hpp

#include <stdio.h>

#include "RMDLFontLoader.h"
#include "RMDLMeshUtils.hpp"
#include "BumpAllocator.hpp"
#include "RMDLMathUtils.hpp"

#define kMaxBuffersInFlight 3

struct UIConfig
{
    NS::UInteger                            screenWidth;
    NS::UInteger                            screenHeight;
    NS::UInteger                            virtualCanvasWidth;
    NS::UInteger                            virtualCanvasHeight;
    FontAtlas                               fontAtlas;
//    FiraCode                                firaCode;
    NS::SharedPtr<MTL::RenderPipelineState> uiPso;
};

struct UIRenderData
{
    std::array<std::unique_ptr<BumpAllocator>, kMaxBuffersInFlight> bufferAllocator;
    std::array<NS::SharedPtr<MTL::Heap>, kMaxBuffersInFlight>       resourceHeaps;
    std::array<NS::SharedPtr<MTL::Buffer>, kMaxBuffersInFlight>     frameDataBuf;
    std::array<NS::SharedPtr<MTL::Buffer>, kMaxBuffersInFlight>     highScorePositionBuf;
    std::array<NS::SharedPtr<MTL::Buffer>, kMaxBuffersInFlight>     currentScorePositionBuf;
    NS::SharedPtr<MTL::Buffer>                                      textureTable;
    NS::SharedPtr<MTL::Buffer>                                      samplerTable;
    NS::SharedPtr<MTL::SamplerState>                                pSampler;
    NS::SharedPtr<MTL::ResidencySet>                                pResidencySet;
};

class RMDLUI
{
public:
    RMDLUI();
    ~RMDLUI();
    void initialize(const UIConfig& config, MTL::Device* pDevice, MTL::CommandQueue* pCommandQueue);
    void showHighScore(const char* label, int highscore, MTL::Device* pDevice);
    void showCurrentScore(const char* label, int score, MTL::Device* pDevice);
    void update(double targetTimestamp, uint8_t frameID);
    void draw(MTL::RenderCommandEncoder* pRenderCmd, uint8_t frameID);
private:
    void createBuffers(MTL::Device* pDevice);
    void createResidencySet(MTL::Device* pDevice, MTL::CommandQueue* pCommandQueue);
    
    UIConfig _uiConfig;
    UIRenderData _renderData;
    IndexedMesh _highScoreMesh;
    IndexedMesh _currentScoreMesh;
    
    simd::float4 _highScorePosition;
    simd::float4 _currentScorePosition;
    double _lastTimestamp = 0.0;
    double _bannerCountdownSecs = 0.0;
};


#endif /* RMDLUI_hpp */
