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

#include <string.h>

#import <Foundation/Foundation.h>

#include "SDL_log.h"

#include "LicenseCheck.h"


#define LICENSECHECK_LOG_1(x) NSLog(@x)
#define LICENSECHECK_LOG_2(x,y) NSLog(@x,y)
#define LICENSECHECK_LOG_3(x,y,z) NSLog(@x,y,z)

// #define USE_PRODUCTION_URL

// validate receipt of purchase
// https://developer.apple.com/library/content/releasenotes/General/ValidateAppStoreReceipt/Chapters/ValidateRemotely.html

static int lastLicenseCheckStartTime = 0;
static int licenseCheckInProgress    = 0;
static int licenseCheckResult        = -1;

NSNumber *resultDetail = NULL;

void LicenseCheckStartAsync(int startTime)
{
    lastLicenseCheckStartTime = startTime;
    licenseCheckInProgress    = 1;
    licenseCheckResult        = -1;
    
    // Load the receipt from the app bundle.
    
    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
    NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];
    if (!receipt)
    {
        LICENSECHECK_LOG_1("Local receipt empty???\n");
        licenseCheckInProgress    = 0;
    }
    else
    {
        NSError *error;
        NSDictionary *requestContents = @{
                                          @"receipt-data": [receipt base64EncodedStringWithOptions:0]
                                          };
        NSData *requestData = [NSJSONSerialization dataWithJSONObject:requestContents
                                                              options:0
                                                                error:&error];
        
        if (!requestData)
        {
            LICENSECHECK_LOG_1("JSON request empty???\n");
            licenseCheckInProgress    = 0;
        }
        else
        {
            // Create a POST request with the receipt data.
#ifdef USE_PRODUCTION_URL
            NSURL *storeURL = [NSURL URLWithString:@"https://buy.itunes.apple.com/verifyReceipt"];
#else
            NSURL *storeURL = [NSURL URLWithString:@"https://sandbox.itunes.apple.com/verifyReceipt"];
#endif
            NSMutableURLRequest *storeRequest = [NSMutableURLRequest requestWithURL:storeURL];
            [storeRequest setHTTPMethod:@"POST"];
            [storeRequest setHTTPBody:requestData];

            // Make a connection to the iTunes Store on a background queue.
            NSOperationQueue *queue = [[NSOperationQueue alloc] init];
            [NSURLConnection sendAsynchronousRequest:storeRequest queue:queue
                completionHandler:^(NSURLResponse *response, NSData *data, NSError *connectionError)
                {
                    if (connectionError) {
                        LICENSECHECK_LOG_1("Connection error???\n");
                        licenseCheckInProgress    = 0;
                    } else {
                        NSError *error;
                        NSDictionary *jsonResponse = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
                        if (!jsonResponse)
                        {
                            LICENSECHECK_LOG_1("JSON response empty???\n");
                            licenseCheckInProgress    = 0;
                        }
                        else
                        {
                            LICENSECHECK_LOG_1("Printing JSON response\n");

                            NSString *jsonString = [[NSString alloc] initWithData:data
                                                                         encoding:NSUTF8StringEncoding];
                            NSLog(@"Response JSON=%@", jsonString);
                            
                            resultDetail = jsonResponse[@"status"];
                            BOOL res = [resultDetail isEqualToNumber:@0];
                            
                            if (res)
                            {
                                licenseCheckResult        = 0;
                            }
                            else
                            {
                                licenseCheckResult        = 1;
                            }
                            
                            licenseCheckInProgress    = 0;
                        }
                    }
                }
            ];
        
        }
    }
}

int LicenseCheckStartTime()
{
    return lastLicenseCheckStartTime;
}

// 0 --> not pending, != 0 --> pending
int LicenseCheckPollStatus()
{
    return licenseCheckInProgress;
}

void LicenseCheckAbort()
{
    lastLicenseCheckStartTime = 0;
    licenseCheckInProgress    = 0;
    licenseCheckResult        = -1;
}

// 0 --> check passed, > 0 --> check failed, < 0 --> inconclusive (not possible to check)
int LicenseCheckResultStatus()
{
    return licenseCheckResult;
}

void LicenseCheckResultDetail(char *str, int maxlen)
{
    if (resultDetail != NULL)
    {
        strncpy(str, [[resultDetail stringValue] UTF8String], maxlen - 1);
        str[maxlen - 1] = 0;
    }
    else
    {
        strcpy(str, "n/a");
    }
}

