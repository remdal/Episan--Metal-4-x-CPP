/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                        +       +          */
/*      File: RMDLGameApplication.h            +++     +++	**/
/*                                        +       +          */
/*      By: Laboitederemdal      **        +       +        **/
/*                                       +           +       */
/*      Created: 27/10/2025 15:33:46      + + + + + +   * ****/
/*                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#import "Cocoa/Cocoa.h"

@interface RMDLGameApplication : NSObject <NSApplicationDelegate, NSWindowDelegate>

- (void)setBrightness:(id)sender;

- (void)setEDRBias:(id)sender;

@end
