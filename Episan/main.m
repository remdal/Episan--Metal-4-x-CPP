//
//  main.cpp
//  VitamineMetalC
//
//  Created by Rémy on 14/11/2025.
//

#import <Cocoa/Cocoa.h>
#import "RMDLGameApplication.h"

static void makeSliderSubmenu(NSMenu* parentMenu, NSString* title, SEL action, double minValue, double maxValue, double defaultValue, NSInteger tickmarks)
{
    NSMenu* submenu = [[NSMenu alloc] initWithTitle:title];
    NSMenuItem* sliderHostItem = [parentMenu addItemWithTitle:title action:nil keyEquivalent:@""];
    sliderHostItem.submenu = submenu;
    NSSlider* slider = [NSSlider sliderWithTarget:nil action:action];
    slider.frame = NSMakeRect(0, 0, 160, 20);
    NSMenuItem* sliderItem = [submenu addItemWithTitle:@"" action:nil keyEquivalent:@""];
    sliderItem.view = slider;
    slider.numberOfTickMarks = tickmarks;
    slider.minValue = minValue;
    slider.maxValue = maxValue;
    slider.intValue = defaultValue;
    slider.allowsTickMarkValuesOnly = tickmarks ? YES : NO;
    slider.autoresizingMask = NSViewMinXMargin|NSViewWidthSizable|NSViewMaxXMargin|NSViewMinYMargin|NSViewMaxYMargin|NSViewHeightSizable;
    [slider sizeToFit];
}

int main( int argc, const char * argv[] )
{
    NSApplication* application = [NSApplication sharedApplication];
    application.mainMenu = [[NSMenu alloc] init];
    //NSBundle *main = [NSBundle mainBundle];
    //NSString *bundleIdentifier = [[NSBundle mainBundle] bundleIdentifier];
    NSString* bundleName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];
    NSMenu* appMenu = [[NSMenu alloc] initWithTitle:@""];
    NSMenuItem* appMenuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    [appMenu addItemWithTitle:[@"About " stringByAppendingString:bundleName] action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:[@"Hide " stringByAppendingString:bundleName] action:@selector(hide:) keyEquivalent:@"h"];
    NSMenuItem* hide_other_item = [appMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
    hide_other_item.keyEquivalentModifierMask = NSEventModifierFlagOption | NSEventModifierFlagCommand;
    [appMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:[@"Quit " stringByAppendingString:bundleName] action:@selector(terminate:) keyEquivalent:@"q"];
    appMenuItem.submenu = appMenu;
    [application.mainMenu addItem:appMenuItem];
    NSMenu* graphicsMenu = [[NSMenu alloc] initWithTitle:@"Graphics"];
    NSMenuItem* graphicsMenuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    graphicsMenuItem.submenu = graphicsMenu;
    makeSliderSubmenu(graphicsMenu, @"Brightness", @selector(setBrightness:), 1, 10, 5, 9);
    makeSliderSubmenu(graphicsMenu, @"EDR Demo", @selector(setEDRBias:), 0, 1, 0, 0);
    [application.mainMenu addItem:graphicsMenuItem];
    NSMenu* windowsMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    NSMenuItem* windowsMenuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    [windowsMenu addItemWithTitle:NSLocalizedString(@"Minimize", @"") action:@selector(performMiniaturize:) keyEquivalent:@"m"];
    windowsMenuItem.submenu = windowsMenu;
    [application.mainMenu addItem:windowsMenuItem];
    application.windowsMenu = windowsMenu;
    RMDLGameApplication* gameApplication = [[RMDLGameApplication alloc] init];
    application.delegate = gameApplication;
    return (NSApplicationMain(argc, argv));
}

//#include <iostream>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    std::cout << "Hello, World!\n";
//    return EXIT_SUCCESS;
//}
//
//int main(int argc, const char * argv[]) {
//    @autoreleasepool {
//        // Setup code that might create autoreleased objects goes here.
//    }
//    return NSApplicationMain(argc, argv);
//}
