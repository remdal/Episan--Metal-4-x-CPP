//
//  RMDLUI.cpp
//  Episan
//
//  Created by Rémy on 12/12/2025.
//

#include "RMDLUI.hpp"

RMDLUI::RMDLUI()
{
    ft_memset(&_highScoreMesh, 0x0, sizeof(IndexedMesh));
    ft_memset(&_currentScoreMesh, 0x0, sizeof(IndexedMesh));
}

RMDLUI::~RMDLUI()
{
    mesh_utils::releaseMesh(&_highScoreMesh);
    mesh_utils::releaseMesh(&_currentScoreMesh);
}

void RMDLUI::showHighScore(const char* label, int highscore, MTL::Device* pDevice)
{
    _bannerCountdownSecs = 5.0f;
    
    float startY = _uiConfig.virtualCanvasHeight * 0.5 * 0.9;
    _highScorePosition = simd_make_float4(0.0, startY, 0, 1);
    
    std::stringstream ss;
    ss << label << highscore;
    
    mesh_utils::releaseMesh(&_highScoreMesh);
    _highScoreMesh = mesh_utils::newTextMesh(ss.str(), _uiConfig.fontAtlas, pDevice);
}

void RMDLUI::showCurrentScore(const char* label, int score, MTL::Device* pDevice)
{
    std::stringstream ss;
    ss << label << score;
    
    const std::string& str = ss.str();
    
    const float strWidth = (float)str.size() * 1.0f;
    
    const float leftSide = (float)_uiConfig.virtualCanvasWidth * -0.5f;
    const float leftMargin = (float)_uiConfig.virtualCanvasWidth * 0.025f;
    
    const float bottomSide = (float)_uiConfig.virtualCanvasHeight * -0.5f;
    const float bottomMargin = leftMargin;
    _currentScorePosition = simd_make_float4(leftSide + leftMargin + strWidth*0.5f,
                                             bottomSide + bottomMargin,
                                             0, 1);
    

    mesh_utils::releaseMesh(&_currentScoreMesh);
    _currentScoreMesh = mesh_utils::newTextMesh(str, _uiConfig.fontAtlas, pDevice);
}

void RMDLUI::update(double targetTimestamp, uint8_t frameID)
{
    if (_lastTimestamp != 0.0)
    {
        double deltat = targetTimestamp - _lastTimestamp;
        _bannerCountdownSecs = std::max(_bannerCountdownSecs - deltat, 0.0);
        
        if (_bannerCountdownSecs <= 0.0 && _highScorePosition.y < _uiConfig.virtualCanvasHeight * 0.5 * 1.2)
        {
            _highScorePosition.y += 1.f * deltat;
        }
    }
    
    _lastTimestamp = targetTimestamp;
    ft_memcpy(_renderData.highScorePositionBuf[frameID]->contents(), &_highScorePosition, sizeof(simd::float4));
    ft_memcpy(_renderData.currentScorePositionBuf[frameID]->contents(), &_currentScorePosition, sizeof(simd::float4));
}

//void RMDLUI::createBuffers(MTL::Device* pDevice)
//{
//    const uint64_t kScratchSize = 1024;
//    auto pHeapDesc = NS::TransferPtr( MTL::HeapDescriptor::alloc()->init() );
//    pHeapDesc->setSize(sizeof(RMDLUniforms::cameraUniforms.projectionMatrix) + 2 * sizeof(simd::float4));
//    pHeapDesc->setStorageMode(MTL::StorageModeShared);
//    
//    for (uint8_t i = 0; i < kMaxFramesInFlight; ++i)
//    {
//        _renderData.bufferAllocator[i] = std::make_unique<BumpAllocator>(pDevice, kScratchSize, MTL::ResourceStorageModeShared);
//        
//        _renderData.resourceHeaps[i] = NS::TransferPtr(pDevice->newHeap(pHeapDesc.get()));
//        
//        _renderData.frameDataBuf[i] = NS::TransferPtr(_renderData.resourceHeaps[i]->newBuffer(sizeof(RMDLUniforms::cameraUniforms.projectionMatrix), MTL::ResourceStorageModeShared));
//        _renderData.frameDataBuf[i]->setLabel(MTLSTR("UI Frame Data Buffer"));
//        auto pFrameData = (RMDLUniforms *)_renderData.frameDataBuf[i]->contents();
//        pFrameData->RMDLUniforms::cameraUniforms.projectionMatrix = math::makeOrtho(-width/2, width/2, height/2, -height/2, -1, 1);
//        
//        _renderData.highScorePositionBuf[i] = NS::TransferPtr(_renderData.resourceHeaps[i]->newBuffer(sizeof(simd::float4), MTL::ResourceStorageModeShared));
//        _renderData.highScorePositionBuf[i]->setLabel(MTLSTR("UI HighScore Position Buffer"));
//        
//        _renderData.currentScorePositionBuf[i] = NS::TransferPtr(_renderData.resourceHeaps[i]->newBuffer(sizeof(simd::float4), MTL::ResourceStorageModeShared));
//        _renderData.currentScorePositionBuf[i]->setLabel(MTLSTR("UI Current Score Position Buffer"));
//    }
//
//}

