#import <CoreHaptics/CoreHaptics.h>
#import <GameController/GameController.h>

@interface LongRunningHaptics : NSObject

- (instancetype)initWithEngine:(CHHapticEngine *)hapticsEngine;

- (void)setIntensity:(float)intensity;

@end
