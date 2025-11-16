#import "Haptics.h"

@interface LongRunningHaptics()
{
    CHHapticEngine*             _engine;
    BOOL                        _isEngineStopped;
    id<CHHapticPatternPlayer>   _longRunningPatternPlayer;
}

- (void)_registerEngineStopHandler;

- (BOOL)_startEngine;

- (void)_startLongRunningEvent;

@end

@implementation LongRunningHaptics

- (instancetype)initWithEngine:(CHHapticEngine *)hapticsEngine
{
    if (self = [super init])
    {
        _engine = hapticsEngine;
        _isEngineStopped = YES;
        [self _startEngine];
        [self _startLongRunningEvent];
    }
    return (self);
}

- (void)_registerEngineStopHandler
{
    __weak LongRunningHaptics* weakSelf = self;
    _engine.stoppedHandler = ^(CHHapticEngineStoppedReason stoppedReason)
    {
        NSLog(@"Haptic Engine Stopped: %d", (int)stoppedReason);
        LongRunningHaptics* strongSelf = weakSelf;
        if (strongSelf)
            strongSelf->_isEngineStopped = YES;
    };
}

- (BOOL)_startEngine
{
    if (_isEngineStopped)
    {
        NSError* __autoreleasing error = nil;
        if ([_engine startAndReturnError:&error])
        {
            NSLog(@"Failed to start haptic Engine, reason : %@", error.localizedDescription);
            return (NO);
        }
        _isEngineStopped = NO;
    }
    return (YES);
}

- (void)_startLongRunningEvent
{
    CHHapticEventParameter* hapticEventParam = [[CHHapticEventParameter alloc] initWithParameterID:CHHapticEventParameterIDHapticIntensity value:1.0];
    CHHapticEvent* continuousEvent = [[CHHapticEvent alloc] initWithEventType:CHHapticEventTypeHapticContinuous parameters:@[hapticEventParam] relativeTime:0 duration:GCHapticDurationInfinite];
    NSError* __autoreleasing error = nil;
    CHHapticPattern* pattern = [[CHHapticPattern alloc] initWithEvents:@[continuousEvent] parameters:@[] error:&error];
    if (!pattern)
    {
        NSLog(@"Error creating haptic pattern: %@", error.localizedDescription);
        assert(pattern);
    }
    assert (_engine);
    _longRunningPatternPlayer = [_engine createPlayerWithPattern:pattern error:&error];
    if (!_longRunningPatternPlayer)
    {
        NSLog(@"Error creating long-running pattern player: %@", error.localizedDescription);
        assert(_longRunningPatternPlayer);
    }
    if (![_longRunningPatternPlayer startAtTime:CHHapticTimeImmediate error:&error])
    {
        NSLog(@"Error starting long-running pattern player: %@", error.localizedDescription);
        assert(false);
    }
    [self setIntensity:0.0f];
}

- (void)setIntensity:(float)intensity
{
    if (_isEngineStopped)
    {
        [self _startEngine];
    }
    if (!_isEngineStopped)
    {
        CHHapticDynamicParameter* hapticParameter = [[CHHapticDynamicParameter alloc] initWithParameterID:CHHapticDynamicParameterIDHapticIntensityControl value:intensity relativeTime:0];
        NSError* __autoreleasing error = nil;
        if (![_longRunningPatternPlayer sendParameters:@[hapticParameter] atTime:CHHapticTimeImmediate error:&error])
        {
            NSLog(@"Error updating motor intensity: %@", error.localizedDescription);
        }
    }
}

@end
