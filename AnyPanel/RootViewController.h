#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>


@interface RootViewController : NSViewController <NSWindowDelegate>
@property (weak) IBOutlet WebView *webView;
@end
