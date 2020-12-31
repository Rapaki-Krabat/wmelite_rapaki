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

void IOS_SendMailWithAttachments(const char** logfiles, const int nrLogFiles, const char** savegames, const int nrSaveGames)
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

-(void) SetSDLWindow:(void*) win
{
    window = (SDL_Window*) win;
}

-(void) SendMailWithAttachments:(const char**)logfiles arg1:(const int)nrLogFiles arg2:(const char**) savegames arg3:(const int) nrSaveGames
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);

    if (SDL_GetWindowWMInfo(window, &info))
    {
        UIWindow* uiWindow = (UIWindow*)info.info.uikit.window;
        UIView* view = [uiWindow.subviews objectAtIndex:0];
        id nextResponder = [view nextResponder];
        if( [nextResponder isKindOfClass:[UIViewController class]] )
        {
            NSLog(@"Found root view controller!");
            UIViewController* rootViewController = (UIViewController*) nextResponder;


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
        }
        else
        {
            NSLog(@"Nextresponder is not of expected class.");
        }
    }
    else
    {
        NSLog(@"GetWindowWMInfo failed!");
    }
}

@end

