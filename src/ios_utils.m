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

#include "ios_utils.h"
#include "DisplayConfig.h"
#include <string.h>
#import <Foundation/NSPathUtilities.h>
#import <UIKit/UIApplication.h>
#include <sys/utsname.h>

void IOS_GetDataDir(char* buffer)
{
	NSString* docsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
	const char* utf8path = [docsDirectory UTF8String];
	strcpy(buffer, utf8path);
}

void IOS_ShowStatusLine(int show)
{
	if (show)
	{
		[[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleBlackTranslucent animated:NO];
		[[UIApplication sharedApplication] setStatusBarHidden:NO animated:UIStatusBarAnimationFade];
	}
	else
	{
		[[UIApplication sharedApplication] setStatusBarHidden:YES animated:UIStatusBarAnimationFade];
	}
}

void IOS_GetDeviceType(char* buffer)
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
		strcpy(buffer, "tablet");
	else
		strcpy(buffer, "phone");
}

NSString *getMachineName()
{
    struct utsname systemInfo;
    if (uname(&systemInfo) < 0) {
        return nil;
    } else {
        return [NSString stringWithCString:systemInfo.machine
                              encoding:NSUTF8StringEncoding];
    }
}

void IOS_GetPlatformIdentifierString(char *buffer)
{
	NSString *ver = [[UIDevice currentDevice] systemVersion];
	NSString* machName = getMachineName();
	const char* utf8path = [machName UTF8String];
	strcpy(buffer, utf8path);
	strcat(buffer, ":");
	const char* utf8path2 = [ver UTF8String];
	strcat(buffer, utf8path2);
}

void IOS_GetAppVersionString(char *buffer)
{
    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
    NSString *majorVersion = [infoDictionary objectForKey:@"CFBundleShortVersionString"];
    NSString *minorVersion = [infoDictionary objectForKey:@"CFBundleVersion"];

    const char* utf8major = [majorVersion UTF8String];
    const char* utf8minor = [minorVersion UTF8String];
    
	strcpy(buffer, utf8major);
	strcat(buffer, ":");
	strcat(buffer, utf8minor);
}

void SetKeepDisplayOn(int keepOn)
{
    if (keepOn != 0)
    {
        [UIApplication sharedApplication].idleTimerDisabled = YES;
    }
    else
    {
        [UIApplication sharedApplication].idleTimerDisabled = NO;
    }
}

