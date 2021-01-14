#include <Cocoa/Cocoa.h>
#include "NativeWindowHelper.h"

void* getNSViewHandle(void* windowHandle) {
    NSWindow* win = (NSWindow*) windowHandle;
    NSView* view = [win contentView];
    return view;
}
