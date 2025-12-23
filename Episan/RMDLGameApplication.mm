/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLGameApplication.m            +++     +++  **/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 27/10/2025 15:36:21      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#import "RMDLGameApplication.h"
#include "RMDLGameCoordinator.hpp"

@interface GameWindow : NSWindow <MTKViewDelegate>

@property (strong) RMDLGameApplication* gameCoordinator;

@end

@implementation GameWindow

- (void)keyDown:(NSEvent *)event
{
    NSString *chars = [event charactersIgnoringModifiers];
    if (chars.length == 0) { return; }
    unichar character = [chars characterAtIndex:0];
    float moveSpeed = 0.065f;
    switch (character)
    {
        case 'w':
            [self.gameCoordinator moveCameraX : 0 Y : 0 Z : -moveSpeed];
            break;
        case 's':
            [self.gameCoordinator moveCameraX : 0 Y : 0 Z : moveSpeed];
            break;
        case 'a':
            [self.gameCoordinator moveCameraX : -moveSpeed Y : 0 Z : 0];
            break;
        case 'd':
            [self.gameCoordinator moveCameraX : moveSpeed Y : 0 Z : 0];
            break;
        case 'q':
            [self.gameCoordinator moveCameraX : 0 Y : moveSpeed Z : 0];
            break;
        case 'e':
            [self.gameCoordinator moveCameraX : 0 Y : -moveSpeed Z : 0];
            break;
        case ' ':
            [self.gameCoordinator moveCameraX : moveSpeed * moveSpeed Y : 0 Z : 0];
            break;
        default:
            [super keyDown:event];
            break;
    }
}

- (void)mouseDragged:(NSEvent *)event
{
    float sensitivity = 0.009f;

    float deltaX = [event deltaX] * sensitivity;
    float deltaY = [event deltaY] * sensitivity;

    [self.gameCoordinator rotateCameraYaw : deltaX Pitch : deltaY];
}

- (void)mouseDown:(NSEvent *)event
{
    [super mouseDown : event];
}

@end

@implementation RMDLGameApplication
{
    GameWindow*                         _window;
    MTKView*                            _pMtkView;
    std::unique_ptr< GameCoordinator >  _pGameCoordinator;
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
    _window.gameCoordinator = self;
    [self updateWindowTitle:_window];
}

- (void)showWindow
{
    [_window setIsVisible:YES];
    [_window makeMainWindow];
    [_window makeKeyAndOrderFront:_window];
    //[_window toggleFullScreen:nil];
}

- (void)createView
{
    NSAssert(_window, @"You need to create the window before the view");

    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    _pMtkView = [[MTKView alloc] initWithFrame:_window.contentLayoutRect device:device];
    _pMtkView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    _pMtkView.colorPixelFormat = MTLPixelFormatRGBA16Float;
    _pMtkView.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
    _pMtkView.clearDepth = 1.0f;
    _pMtkView.framebufferOnly = YES;
    _pMtkView.paused = NO;
    _pMtkView.delegate = self;
    _pMtkView.enableSetNeedsDisplay = NO;

    NSScreen *screen = _window.screen ?: [NSScreen mainScreen];
    CGFloat scale = screen.backingScaleFactor ?: 1.0;
    CGSize sizePts = _pMtkView.bounds.size;
    _pMtkView.drawableSize = CGSizeMake(sizePts.width * scale, sizePts.height * scale);

    _window.contentView = _pMtkView;

    MTL::Device* m_pDevice = (__bridge MTL::Device *)device;
    MTL::PixelFormat pixelFormat = MTL::PixelFormatRGBA16Float;
    NS::UInteger w = (NS::UInteger)_pMtkView.drawableSize.width;
    NS::UInteger h = (NS::UInteger)_pMtkView.drawableSize.height;
    NSUInteger gameUICanvasSize = 30;
    NSString* shaderPath = NSBundle.mainBundle.resourcePath;
//        NSString *shaderPath = @"background_general.png";

    _pGameCoordinator = std::make_unique<GameCoordinator>(m_pDevice, pixelFormat, w, h, gameUICanvasSize, std::string{""});
}

- (void)updateDrawableSizeForCurrentWindow
{
    if (!_pMtkView) { return; }
    NSScreen *screen = _window.screen ?: [NSScreen mainScreen];
    CGFloat scale = screen.backingScaleFactor ?: 1.0;
    CGSize sizePts = _pMtkView.bounds.size;
    _pMtkView.drawableSize = CGSizeMake(sizePts.width * scale, sizePts.height * scale);
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
    [self showWindow];
    [self evaluateCommandLine];
    [self updateMaxEDRValue];
    [NSNotificationCenter.defaultCenter addObserver:self selector:@selector(maxEDRValueDidChange:) name:NSApplicationDidChangeScreenParametersNotification object:nil];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    _pGameCoordinator.reset();
}

- (void)updateMaxEDRValue
{
    float maxEDRValue = _window.screen.maximumExtendedDynamicRangeColorComponentValue;
    [self setMaxEDRValue:maxEDRValue];
    [self updateWindowTitle:_window];
}

- (void)maxEDRValueDidChange:(NSNotification *)notification
{
    [self updateMaxEDRValue];
}

- (void)updateWindowTitle:(nonnull NSWindow *) window
{
    NSScreen* screen = window.screen;
    NSString* title = [NSString stringWithFormat:@"Jeu de la Vie Metal4 x C++ (%@ @ %ldHz, EDR max: %.2f) (Ça s'appelle vrmt comme ça)", screen.localizedName, (long)screen.maximumFramesPerSecond, screen.maximumExtendedDynamicRangeColorComponentValue];
    window.title = title;
}

- (void)windowDidChangeScreen:(NSNotification *)notification
{
    [self updateMaxEDRValue];
    [self updateDrawableSizeForCurrentWindow];
}

- (void)windowDidResize:(NSNotification *)notification
{
    [self updateDrawableSizeForCurrentWindow];
}

- (void)setBrightness:(id)sender
{
    NSSlider* slider = (NSSlider *)sender;
    float brightness = slider.floatValue * 100;
    [self setBrightnessValue:brightness];
}

- (void)setEDRBias:(id)sender
{
    NSSlider* slider = (NSSlider *)sender;
    float edrBias = slider.floatValue;
    [self setEDRBiasValue:edrBias];
}

#pragma mark - API appelée par GameWindow (pont vers C++)

- (void)moveCameraX:(float)x Y:(float)y Z:(float)z
{
    _pGameCoordinator->moveCamera( simd::float3 {x, y, z} );
}

- (void)rotateCameraYaw:(float)yaw Pitch:(float)pitch
{
    _pGameCoordinator->rotateCamera(yaw, pitch);
}

- (void)setBrightnessValue:(float)brightness
{
    _pGameCoordinator->setBrightness(brightness);
}

- (void)setEDRBiasValue:(float)edrBias
{
    _pGameCoordinator->setEDRBias(edrBias);
}

- (void)setMaxEDRValue:(float)value
{
    _pGameCoordinator->setMaxEDRValue(value);
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    if (_pGameCoordinator) _pGameCoordinator->draw((__bridge MTK::View *)view);
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
//    _pGameCoordinator->resizeMtkView((__bridge NS::UInteger *)(NS::UInteger)size.width, (__bridge NS::UInteger *)(NS::UInteger)size.height);
    _pGameCoordinator->resizeMtkView((NS::UInteger)size.width, (NS::UInteger)size.height);
}

//- (BOOL)commitEditingAndReturnError:(NSError *__autoreleasing  _Nullable * _Nullable)error { 
//
//}

- (void)encodeWithCoder:(nonnull NSCoder *)coder { 
    
}

- (void)mouseUp:(NSEvent *)event {
}

- (void)otherMouseDown:(NSEvent *)event {
}

- (void)otherMouseUp:(NSEvent *)event {
}

- (void)rightMouseDown:(NSEvent *)event {
}

- (void)rightMouseUp:(NSEvent *)event {
}

- (void)scrollWheel:(NSEvent *)event {
}

- (void)updateTrackingAreas {
}

- (void)windowWillClose:(NSNotification *)notification {
}

- (void)keyUp:(NSEvent *)event {
}

@end

