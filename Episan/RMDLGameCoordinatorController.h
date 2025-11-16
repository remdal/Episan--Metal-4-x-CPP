/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLGameCoordinatorController.h   +++     +++ **/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 27/10/2025 15:41:47      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#import <QuartzCore/QuartzCore.h>

@interface RMDLGameCoordinatorController : NSObject <CAMetalDisplayLinkDelegate>

- (nonnull instancetype)initWithMetalLayer:(nonnull CAMetalLayer *)metalLayer gameUICanvasSize:(NSUInteger)gameUICanvasSize;

- (void)metalDisplayLink:(nonnull CAMetalDisplayLink *)link needsUpdate:(nonnull CAMetalDisplayLinkUpdate *)update;

- (void)maxEDRValueDidChangeTo:(float)value;

- (void)setBrightness:(float)brightness;

- (void)setEDRBias:(float)edrBias;

- (void)moveCameraX:(float)x Y:(float)y Z:(float)z;

- (void)rotateCameraYaw:(float)yaw Pitch:(float)pitch;

- (void)updateCameraAspectRatio:(float)aspectRatio;

@end
