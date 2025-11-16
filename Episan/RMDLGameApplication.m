/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLGameApplication.m            +++     +++	**/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 27/10/2025 15:36:21      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

#import "RMDLGameCoordinatorController.h"
#import "RMDLGameApplication.h"

@interface GameWindow : NSWindow

@property (strong) RMDLGameCoordinatorController* gameCoordinator;

@end

@implementation GameWindow

- (void)keyDown:(NSEvent *)event
{
    NSString *chars = [event charactersIgnoringModifiers];
    unichar character = [chars characterAtIndex:0];
    float moveSpeed = 0.065f;
    switch (character)
    {
        case 'w':
            [self.gameCoordinator moveCameraX:0 Y:0 Z:-moveSpeed];
            break;
        case 's':
            [self.gameCoordinator moveCameraX:0 Y:0 Z:moveSpeed];
            break;
        case 'a':
            [self.gameCoordinator moveCameraX:-moveSpeed Y:0 Z:0];
            break;
        case 'd':
            [self.gameCoordinator moveCameraX:moveSpeed Y:0 Z:0];
            break;
        case 'q':
            [self.gameCoordinator moveCameraX:0 Y:moveSpeed Z:0];
            break;
        case 'e':
            [self.gameCoordinator moveCameraX:0 Y:-moveSpeed Z:0];
            break;
        case ' ':
            [self.gameCoordinator moveCameraX:moveSpeed * moveSpeed Y:0 Z:0];
            break;
    }
}

- (void)mouseDragged:(NSEvent *)event
{
    float sensitivity = 0.009f;
    float deltaX = [event deltaX] * sensitivity;
    float deltaY = [event deltaY] * sensitivity;
    
    [self.gameCoordinator rotateCameraYaw:deltaX Pitch:deltaY];
}

- (void)mouseDown:(NSEvent *)event
{
}

@end

@implementation RMDLGameApplication
{
    GameWindow*                     _window;
    NSView*                         _view;
    CAMetalLayer*                   _metalLayer;
    RMDLGameCoordinatorController*  _gameCoordinator;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return (YES);
}

- (void)createWindow
{
    NSWindowStyleMask mask = NSWindowStyleMaskClosable | NSWindowStyleMaskTitled | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
    NSScreen * screen = [NSScreen mainScreen];
    NSRect contentRect = NSMakeRect(0, 0, 1280, 720);
    contentRect.origin.x = (screen.frame.size.width / 2) - (contentRect.size.width / 2);
    contentRect.origin.y = (screen.frame.size.height / 2) - (contentRect.size.height / 2);
    _window = [[GameWindow alloc] initWithContentRect:contentRect styleMask:mask backing:NSBackingStoreBuffered defer:NO screen:screen];
    _window.releasedWhenClosed = NO;
    _window.minSize = NSMakeSize(640, 360);
    _window.delegate = self;
    [self updateWindowTitle:_window];
}

- (void)showWindow
{
    [_window setIsVisible:YES];
    [_window makeMainWindow];
    [_window makeKeyAndOrderFront:_window];
    [_window toggleFullScreen:nil];
}
- (void)createView
{
    NSAssert(_window, @"You need to create the window before the view");
    _metalLayer = [[CAMetalLayer alloc] init];
    _metalLayer.device = MTLCreateSystemDefaultDevice();
    _metalLayer.drawableSize = NSMakeSize(1280, 720);
    _metalLayer.opaque = YES;
    _metalLayer.cornerRadius = 10;
    _metalLayer.framebufferOnly = YES;
    _metalLayer.contentsGravity = kCAGravityResizeAspect;
    _metalLayer.backgroundColor = CGColorGetConstantColor(kCGColorWhite);
    _metalLayer.pixelFormat = MTLPixelFormatRGBA16Float;
    _metalLayer.wantsExtendedDynamicRangeContent = YES;
    _metalLayer.colorspace = CGColorSpaceCreateWithName(kCGColorSpaceExtendedLinearDisplayP3);
    _view = [[NSView alloc] initWithFrame:_window.contentLayoutRect];
    _view.layer = _metalLayer;
    _window.contentView = _view;
}

- (void)createGame
{
    NSAssert(_metalLayer, @"You need to create the MetalLayer before creating the game");
    NSUInteger gameUICanvasSize = 30;
    _gameCoordinator = [[RMDLGameCoordinatorController alloc] initWithMetalLayer:_metalLayer gameUICanvasSize:gameUICanvasSize];
    _window.gameCoordinator = _gameCoordinator;
}

- (void)evaluateCommandLine
{
    NSArray<NSString *>* args = [[NSProcessInfo processInfo] arguments];
    BOOL exitAfterOneFrame = [args containsObject:@"--auto-close"];
    if (exitAfterOneFrame)
    {
        NSLog(@"Automatically terminating in 8 seconds...");
        [[NSApplication sharedApplication] performSelector:@selector(terminate:) withObject:self afterDelay:8];
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [self createWindow];
    [self createView];
    [self createGame];
    [self showWindow];
    [self evaluateCommandLine];
    [self updateMaxEDRValue];
    [NSNotificationCenter.defaultCenter addObserver:self selector:@selector(maxEDRValueDidChange:) name:NSApplicationDidChangeScreenParametersNotification object:nil];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    self->_gameCoordinator = nil;
}

- (void)updateMaxEDRValue
{
    float maxEDRValue = _window.screen.maximumExtendedDynamicRangeColorComponentValue;
    [_gameCoordinator maxEDRValueDidChangeTo:maxEDRValue];
    [self updateWindowTitle:_window];
}

- (void)maxEDRValueDidChange:(NSNotification *)notification
{
    [self updateMaxEDRValue];
}

- (void)updateWindowTitle:(nonnull NSWindow *) window
{
    NSScreen* screen = window.screen;
    NSString* title = [NSString stringWithFormat:@"Padentvo Metal C++ (%@ @ %ldHz, EDR max: %.2f)", screen.localizedName, (long)screen.maximumFramesPerSecond, screen.maximumExtendedDynamicRangeColorComponentValue];
    window.title = title;
}

- (void)windowDidChangeScreen:(NSNotification *)notification
{
    [self updateMaxEDRValue];
}

- (void)setBrightness:(id)sender
{
    NSSlider* slider = (NSSlider *)sender;
    float brightness = slider.floatValue * 100;
    NSAssert(_gameCoordinator, @"Game coordinator needs to be initialized to set brightness");
    [_gameCoordinator setBrightness:brightness];
}

- (void)setEDRBias:(id)sender
{
    NSSlider* slider = (NSSlider *)sender;
    float edrBias = slider.floatValue;
    NSAssert(_gameCoordinator, @"Game coordinator needs to be initialized to set EDR bias");
    [_gameCoordinator setEDRBias:edrBias];
}

@end
