#include <AppKit/AppKit.h>
#include <QuartzCore/CAMetalLayer.h>

extern "C" id createMetalLayer(NSWindow* window)
{
    [window.contentView setWantsLayer:YES];
    id metalLayer = [CAMetalLayer layer];
    [window.contentView setLayer:metalLayer];

    return metalLayer;
}