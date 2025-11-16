#import <PHASE/PHASE.h>

#include "RMDLPhaseAudio.hpp"

PhaseAudio::PhaseAudio( const std::string& assetSearchPath )
{
    PHASEEngine* engine = [[PHASEEngine alloc] initWithUpdateMode:PHASEUpdateModeAutomatic];
    
    NSError __autoreleasing* error = nil;
    if (![engine startAndReturnError:&error])
    {
        NSLog(@"Error starting PHASE engine: %@", error.localizedDescription);
        assert(false);
    }
    // Transfer ownership of these engine objects from Objective-C ARC to manual management,
    // so the destructor releases it at shutdown time:
    _phaseEngine = CFBridgingRetain(engine);
}

PhaseAudio::~PhaseAudio()
{
    PHASEEngine* engine = (__bridge PHASEEngine *)_phaseEngine;
    [engine stop];
    for (auto&& soundDatum : soundData)
    {
        CFRelease(soundDatum.event);
        CFRelease(soundDatum.asset);
        [engine.assetRegistry unregisterAssetWithIdentifier:[NSString stringWithUTF8String:soundDatum.assetIdentifier.c_str()] completion:nil];
        [engine.assetRegistry unregisterAssetWithIdentifier:[NSString stringWithUTF8String:soundDatum.eventIdentifier.c_str()] completion:nil];
    }
    CFRelease(_phaseEngine);
}

std::string PhaseAudio::loadStereoSound( const std::string &assetSearchPath, const std::string &assetName )
{
    PHASEEngine* engine = (__bridge PHASEEngine *)_phaseEngine;
    assert(engine || !"Engine not available for loading sounds");

    // Load and register sound asset:
    NSError* __autoreleasing error = nil;
    NSString* urlString = [NSString stringWithFormat:@"%s/%s",
                           assetSearchPath.c_str(),
                           assetName.c_str()];

    NSURL* url = [NSURL URLWithString:urlString];
    NSString* assetIdentifier = [NSString stringWithUTF8String:assetName.c_str()];
    PHASESoundAsset* soundAsset = [engine.assetRegistry registerSoundAssetAtURL:url
                                                                     identifier:assetIdentifier
                                                                      assetType:PHASEAssetTypeResident
                                                                  channelLayout:nil
                                                              normalizationMode:PHASENormalizationModeDynamic
                                                                          error:&error];

    if (!soundAsset)
    {
        NSLog(@"Error registering sound asset %@: %@", urlString, error.localizedDescription);
        assert(false);
    }

    // Register sound event:
    AVAudioChannelLayout* channelLayout =
    [[AVAudioChannelLayout alloc] initWithLayoutTag:kAudioChannelLayoutTag_Stereo];
    
    PHASEChannelMixerDefinition* channelMixerDefinition =
    [[PHASEChannelMixerDefinition alloc] initWithChannelLayout:channelLayout];
    
    PHASESamplerNodeDefinition* samplerNodeDefinition =
    [[PHASESamplerNodeDefinition alloc] initWithSoundAssetIdentifier:[NSString stringWithUTF8String:assetName.c_str()]
                                                     mixerDefinition:channelMixerDefinition];
    
    samplerNodeDefinition.playbackMode = PHASEPlaybackModeOneShot;
    [samplerNodeDefinition setCalibrationMode:PHASECalibrationModeRelativeSpl level:0];
    
    NSString* eventIdentifier = [NSString stringWithFormat:@"%s_event", assetName.c_str()];
    PHASESoundEventNodeAsset* soundEventAsset =
    [engine.assetRegistry registerSoundEventAssetWithRootNode:samplerNodeDefinition
                                                   identifier:eventIdentifier
                                                        error:&error];
    
    if (!soundEventAsset)
    {
        NSLog(@"Error creating sound event for asset: %@", error.localizedDescription);
        assert(false);
    }
    
    // Save sound asset and event for releasing durning shutdown
    // CFBridgingRetain transfers ownership from ARC to manual management,
    // so the destructor can release these assets deliberately.
    soundData.emplace_back( SoundDatum {
        CFBridgingRetain(soundAsset),
        CFBridgingRetain(soundEventAsset),
        assetIdentifier.UTF8String,
        eventIdentifier.UTF8String
    });
    return ( std::string(assetIdentifier.UTF8String) );
}

void PhaseAudio::playSoundEvent( const std::string& identifier )
{
    NSError* __autoreleasing error = nil;
    PHASEEngine* engine = (__bridge PHASEEngine *)_phaseEngine;
    PHASESoundEvent* soundEvent = [[PHASESoundEvent alloc] initWithEngine:engine
                                                          assetIdentifier:[NSString stringWithFormat:@"%s_event", identifier.c_str()]
                                                                    error:&error];

    if (!soundEvent)
    {
        NSLog(@"Unable to play sound event with identifier: %s", identifier.c_str());
        assert(false);
    }
    [soundEvent startWithCompletion:nil];
}
