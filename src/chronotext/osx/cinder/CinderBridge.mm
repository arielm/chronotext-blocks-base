/*
 * THE NEW CHRONOTEXT TOOLKIT: https://github.com/arielm/new-chronotext-toolkit
 * COPYRIGHT (C) 2012, ARIEL MALKA ALL RIGHTS RESERVED.
 *
 * THE FOLLOWING SOURCE-CODE IS DISTRIBUTED UNDER THE SIMPLIFIED BSD LICENSE:
 * https://github.com/arielm/new-chronotext-toolkit/blob/master/LICENSE.md
 */

#import "chronotext/osx/cinder/CinderBridge.h"

using namespace std;
using namespace ci;
using namespace chr;

@implementation CinderBridge

@synthesize cinderDelegate;
@synthesize view;

- (id) initWithDelegate:(CinderDelegate*)delegate
{
    if (self = [super init])
    {
        cinderDelegate = delegate;

        assert(delegate->getWindow());
        view = reinterpret_cast<NSView*>(delegate->getWindow()->getNative());
    }
    
    return self;
}

- (void) sendMessageToSketch:(int)what
{
    cinderDelegate->sendMessageToSketch(what);
}

- (void) sendMessageToSketch:(int)what json:(id)json
{
    NSData *data = [NSJSONSerialization dataWithJSONObject:json options:0 error:nil];
    NSString *string = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
    
    cinderDelegate->sendMessageToSketch(what, [string UTF8String]);
}

- (void) sendMessageToSketch:(int)what body:(NSString*)body
{
    cinderDelegate->sendMessageToSketch(what, [body UTF8String]);
}

- (void) handleMessageFromSketch:(int)what body:(NSString*)body
{}

@end