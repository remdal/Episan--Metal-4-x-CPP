#ifndef RMDLCONTROLLER_H
#define RMDLCONTROLLER_H

#include <CoreFoundation/CFBase.h>
#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>
#import <GameController/GameController.h>

#include <unordered_map>

#include <simd/simd.h>

#include "NonCopyable.h"
#import "Haptics.h"

namespace MTL
{
    class RenderCommandEncoder;
}

class GameController : public NonCopyable
{
public:
    GameController();
    ~GameController();

    bool isLeftArrowDown() const;
    bool isRightArrowDown() const;
    bool isXMacDown() const;
    bool isSpacebarDown() const;

    float leftThumbstickX() const;
    float rightThumbstickX() const;
    bool isButtonADown() const;
    bool isButtonBDown() const;
    bool isButtonXDown() const;
    bool isButtonYDown() const;

    void setHapticIntensity(float intensity) const;
    
    void  renderOverlay( MTL::RenderCommandEncoder* pEnc );
    
    simd::float3 accelerometerData() const;
private:
    CFTypeRef   _haptics;
    CFTypeRef   _motionManager;
};

#endif // RMDLCONTROLLER_H
