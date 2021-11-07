/*
 This file is part of WME Lite.
 http://dead-code.org/redir.php?target=wmelite
 
 Copyright (c) 2011 Jan Nedoma
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#import <UIKit/UIViewController.h>
#import <UIKit/UIPopoverPresentationController.h>
#include "ios_sendmail.h"
#include "SendEmailIOSITF.h"
#include "SDL_syswm.h"

void IOS_SendMailWithAttachments(char** logfiles, const int nrLogFiles, char** savegames, const int nrSaveGames)
{
    [[SendEmailMgr sharedSendEmailMgr] SendMailWithAttachments:logfiles arg1:nrLogFiles arg2:savegames arg3:nrSaveGames];
}

void IOS_SetWindowForMailDialogue(SDL_Window* wind)
{
    [[SendEmailMgr sharedSendEmailMgr] SetSDLWindow:wind];
}

@implementation SendEmailMgr

static SendEmailMgr* _sharedSendEmailMgr = nil;

SDL_Window* window;

+ (SendEmailMgr*)sharedSendEmailMgr
{
    @synchronized([SendEmailMgr class])
    {
        if (!_sharedSendEmailMgr)
            [[self alloc] init];
        
        return _sharedSendEmailMgr;
    }
    return nil;
}

+(id)alloc
{
    @synchronized([SendEmailMgr class])
    {
        NSAssert(_sharedSendEmailMgr == nil, @"Attempted to allocate a second instance of a singleton.");
        _sharedSendEmailMgr = [super alloc];
        return _sharedSendEmailMgr;
    }
    return nil;
}

-(id) init
{
    return self;
}

- (void) dealloc
{
    [super dealloc];
}

-(void) SetSDLWindow:(void*) win
{
    window = (SDL_Window*) win;
}

-(void) SendMailWithAttachments:(char**)logfiles arg1:(const int)nrLogFiles arg2:(char**) savegames arg3:(const int) nrSaveGames
{
    SDL_SysWMinfo systemWindowInfo;
    SDL_VERSION(&systemWindowInfo.version);

    if (SDL_GetWindowWMInfo(window, &systemWindowInfo))
    {
        UIWindow * appWindow = systemWindowInfo.info.uikit.window;
        UIViewController * rootViewController = appWindow.rootViewController;
        
        NSString *myString = @"Log files and savegames.";
        
        NSMutableArray *activityItems = [[NSMutableArray alloc] init];
        
        [activityItems addObject:myString];
        
        NSLog(@"Nr logfiles: %d", nrLogFiles);
        
        for (int i = 0; i < nrLogFiles; i++)
        {
            NSString *filepath = @(logfiles[i]);
            
            NSLog(@"Adding file=%@", filepath);
            
            // NSData *myData = [NSData dataWithContentsOfFile:filepath];
            
            NSURL *myUrl = [NSURL fileURLWithPath:filepath isDirectory:NO];
            
            [activityItems addObject:myUrl]; // was myData
        }
        
        NSLog(@"Nr savegames: %d", nrSaveGames);
        
        for (int i = 0; i < nrSaveGames; i++)
        {
            NSString *filepath = @(savegames[i]);
            
            NSLog(@"Adding file=%@", filepath);
            
            // NSData *myData = [NSData dataWithContentsOfFile:filepath];
            
            NSURL *myUrl = [NSURL fileURLWithPath:filepath isDirectory:NO];
            
            [activityItems addObject:myUrl]; // was myData
        }
        
        UIActivityViewController *activityViewControntroller = [[UIActivityViewController alloc] initWithActivityItems:activityItems applicationActivities:nil];
        activityViewControntroller.excludedActivityTypes = @[];
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
            activityViewControntroller.popoverPresentationController.sourceView = rootViewController.view;
            activityViewControntroller.popoverPresentationController.sourceRect = CGRectMake(rootViewController.view.bounds.size.width/2, rootViewController.view.bounds.size.height/4, 0, 0);
        }
        [rootViewController presentViewController:activityViewControntroller animated:true completion:nil];
        
        // [activityItems release];
        
#if 0
        UIWindow* appWindow = (UIWindow*)info.info.uikit.window;
        
        UIViewController* rootViewController = appWindow.rootViewController;
        
        UIViewController *contentViewController = [[UIViewController alloc] init];

        // Present the view controller using the popover style.
        contentViewController.modalPresentationStyle = UIModalPresentationPopover;
        [rootViewController presentViewController:contentViewController
                           animated:YES
                         completion:nil];

        // Get the popover presentation controller and configure it.
        UIPopoverPresentationController *presentationController =[contentViewController popoverPresentationController];
        presentationController.permittedArrowDirections = UIPopoverArrowDirectionUp;
        presentationController.sourceView = rootViewController.view;
        presentationController.sourceRect = rootViewController.view.frame;
#endif
        
    }
    else
    {
        NSLog(@"GetWindowWMInfo failed!");
    }
}

@end

