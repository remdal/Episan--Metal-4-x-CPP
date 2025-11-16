#ifndef RMDLPHASEAUDIO_HPP
#define RMDLPHASEAUDIO_HPP

#include <CoreFoundation/CFBase.h>
#include <string>
#include <vector>

#include "NonCopyable.h"

class PhaseAudio : public NonCopyable
{
public:
    PhaseAudio( const std::string& assetSearchPath );
    ~PhaseAudio();
    
    std::string loadStereoSound( const std::string& assetSearchPath, const std::string& assetName );
    void playSoundEvent( const std::string& identifier );
private:
    struct SoundDatum
    {
        CFTypeRef   asset;
        CFTypeRef   event;
        std::string assetIdentifier;
        std::string eventIdentifier;
    };
    
    std::vector<SoundDatum> soundData;
    CFTypeRef               _phaseEngine;
};

#endif // RMDLPHASEAUDIO_HPP
