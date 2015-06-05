/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012-2015, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#import <GLKit/GLKit.h>

OBJC_EXPORT NSString* GLViewControllerRenderingAPIKey;
OBJC_EXPORT NSString* GLViewControllerPreferredFramesPerSecondKey;
OBJC_EXPORT NSString* GLViewControllerMultipleTouchEnabledKey;
OBJC_EXPORT NSString* GLViewControllerInterfaceOrientationMaskKey;
OBJC_EXPORT NSString* GLViewControllerColorFormatKey;
OBJC_EXPORT NSString* GLViewControllerDepthFormatKey;
OBJC_EXPORT NSString* GLViewControllerStencilFormatKey;
OBJC_EXPORT NSString* GLViewControllerMultisampleKey;

@class CinderBridge;

enum
{
    REASON_VIEW_WILL_APPEAR,
    REASON_VIEW_WILL_DISAPPEAR,
    REASON_APP_DID_BECOME_ACTIVE,
    REASON_APP_WILL_RESIGN_ACTIVE,
};

@interface GLViewController : GLKViewController
{
    CinderBridge *cinderBridge;
    GLKView *glView;
}

@property (nonatomic, readonly) CinderBridge *cinderBridge;
@property (nonatomic, readonly) GLKView *glView;
@property (nonatomic, readonly) BOOL appeared;

- (id) initWithBridge:(CinderBridge*)bridge properties:(NSDictionary*)properties;

@end
