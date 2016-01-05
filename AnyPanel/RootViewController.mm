#import "RootViewController.h"

#include "anypanel.h"

@interface RootViewController ()
{
    AnyPanel *_anyPanel;
    NSTimer *_pollTimer;
    NSTimer *_checkPreferencesTimer;
}
@end

@implementation RootViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    NSWindow *window = self.view.window;
    
    [window setDelegate:self];
    [window setStyleMask:NSBorderlessWindowMask];
    [window setIgnoresMouseEvents:YES];
    [window setLevel: NSStatusWindowLevel];
    [window setOpaque:NO];

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSArray *appSupportDir = [fileManager URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask];
    NSURL *dirPath = [[appSupportDir objectAtIndex:0] URLByAppendingPathComponent:@"AnyPanel"];
    [fileManager createDirectoryAtURL:dirPath withIntermediateDirectories:YES attributes:nil error:nil];
    
    _anyPanel = new AnyPanel(dirPath.path.UTF8String);
    
    [self checkPreferences];
    
    _pollTimer = [NSTimer scheduledTimerWithTimeInterval:1.0 / 60.0
                                                  target:self
                                                selector:@selector(poll)
                                                userInfo:nil
                                                 repeats:YES];

    _checkPreferencesTimer = [NSTimer scheduledTimerWithTimeInterval:1.0
                                                              target:self
                                                            selector:@selector(checkPreferences)
                                                            userInfo:nil
                                                             repeats:YES];
}

- (void)windowWillClose:(NSNotification *)notification
{
    [_pollTimer invalidate];
    _pollTimer = nil;

    [_checkPreferencesTimer invalidate];
    _checkPreferencesTimer = nil;
    
    delete _anyPanel;
    _anyPanel = NULL;
}

- (void)poll
{
    std::vector<std::string> commands = _anyPanel->poll();
    for (std::size_t i = 0; i < commands.size(); ++i)
        [_webView stringByEvaluatingJavaScriptFromString:[NSString stringWithUTF8String:commands[i].c_str()]];
}

- (void)checkPreferences
{
    if (_anyPanel->loadPreferences())
    {
        NSWindow *window = self.view.window;
        
        ARect rect = _anyPanel->geometry();
        NSRect screenFrame = [[NSScreen mainScreen] frame];
        [window setFrame:NSMakeRect(rect.x, screenFrame.size.height - rect.height - rect.y, rect.width, rect.height) display:YES];
        window.alphaValue = _anyPanel->opacity();

        [_webView.mainFrame loadHTMLString:[NSString stringWithUTF8String:_anyPanel->generateHtml().c_str()] baseURL:nil];
    }
}

@end