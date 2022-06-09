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

#import "IOS_StoreKit.h"

// #import "SDL_Log.h"

// #define STOREKIT_LOG SDL_Log
// #define STOREKIT_LOG(...)
#define STOREKIT_LOG_1(x) NSLog(@x)
#define STOREKIT_LOG_2(x,y) NSLog(@x,y)

@implementation StoreKitMgr

// C interface
void StoreKit_SetExternalData(void* data)
{
    [[StoreKitMgr sharedStoreKitMgr] setExternalData:data];
}

int StoreKit_IsStoreAvailable()
{
    if ([[StoreKitMgr sharedStoreKitMgr] storeAvailable]) return 1;
    else return 0;
}

void StoreKit_EnableEvents()
{
    [[StoreKitMgr sharedStoreKitMgr] enableEvents];
}

void StoreKit_DisableEvents()
{
    [[StoreKitMgr sharedStoreKitMgr] disableEvents];    
}

void StoreKit_ValidateProducts(const char* ids, TTextEncoding encoding)
{
    [[StoreKitMgr sharedStoreKitMgr] validateProducts:ids];
}

void StoreKit_Purchase(const char* prodId, TTextEncoding encoding)
{
    [[StoreKitMgr sharedStoreKitMgr] purchase:prodId];
}

void StoreKit_RestoreTransactions(TTextEncoding encoding)
{
    [[StoreKitMgr sharedStoreKitMgr]restoreTransactions];
}

int StoreKit_FinishTransaction(const char* transId, TTextEncoding encoding)
{
    if ([[StoreKitMgr sharedStoreKitMgr]finishTransaction:transId]) return 1;
    else return 0;
}



static StoreKitMgr* _sharedStoreKitMgr = nil;

+ (StoreKitMgr*)sharedStoreKitMgr
{
	@synchronized([StoreKitMgr class])
	{
		if (!_sharedStoreKitMgr)
			[[self alloc] init];
		
		return _sharedStoreKitMgr;
	}
	return nil;
}

+(id)alloc
{
	@synchronized([StoreKitMgr class])
	{
		NSAssert(_sharedStoreKitMgr == nil, @"Attempted to allocate a second instance of a singleton.");
		_sharedStoreKitMgr = [super alloc];
		return _sharedStoreKitMgr;
	}
	return nil;
}

-(id) init
{
    dictTransactions = [[NSMutableDictionary alloc] init];
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

-(void)setExternalData:(void*)data
{
    externalData = data;
}

-(BOOL)storeAvailable
{
    return [SKPaymentQueue canMakePayments];
}

-(void)enableEvents
{
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
}

-(void)disableEvents
{
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
}

- (void)productsRequest:(SKProductsRequest*)request didReceiveResponse:(SKProductsResponse*)response
{
    StoreKit_ReceiveProductsStartCallback(externalData);
    
    NSArray* products = response.products;
	for (SKProduct* product in products)
	{
        STOREKIT_LOG_2("Valid Product: %s\n", [product.productIdentifier UTF8String]);
        
        StoreKit_AddValidProductCallback([product.productIdentifier UTF8String], [product.localizedTitle UTF8String], [product.localizedDescription UTF8String], [product.localizedPrice UTF8String], externalData);
    }
    
    NSArray* invalid = response.invalidProductIdentifiers;
	for (NSString* invalidId in invalid)
	{
        STOREKIT_LOG_2("Invalid Product: %s\n", [invalidId UTF8String]);
        
        StoreKit_AddInvalidProductCallback([invalidId UTF8String], externalData);
    }
    
    StoreKit_ReceiveProductsEndCallback(externalData);
}

- (void)paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions
{
    //[dictTransactions removeAllObjects];
    
    
    for (SKPaymentTransaction* transaction in transactions)
    {
        // according to docs, transactionIdentifier is only
        // valid in states "purchased" and "restored"
        // thus don't try to print it when in any other state
        //
        // strange enough that the original code tried to read this field
        // also when in failed state - we rather use a dummy
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
            case SKPaymentTransactionStateRestored:
                STOREKIT_LOG_2("Transaction ID recorded from update call: %s.\n", [transaction.transactionIdentifier UTF8String]);
                [dictTransactions setObject:transaction forKey:transaction.transactionIdentifier];
                break;
            case SKPaymentTransactionStateFailed:
                STOREKIT_LOG_1("Transaction ID NOT recorded from update call (failed state)");
                // do not fake a transaction with dummy data, it adds no value and cannot be finished anyway 
                // [dictTransactions setObject:transaction forKey:@"transFailed"];
                break;
            case SKPaymentTransactionStatePurchasing:
                STOREKIT_LOG_1("Transaction ID NOT recorded from update call (purchasing state)");
                break;
            case SKPaymentTransactionStateDeferred:
                STOREKIT_LOG_1("Transaction ID NOT recorded from update call (deferred state).\n");
                break;
        }
    }
    
    StoreKit_ReceiveTransactionsStartCallback(externalData);
    
    NSArray* keys = [dictTransactions allKeys];
	for (NSString* key in keys)
    {
        SKPaymentTransaction* transaction = [dictTransactions objectForKey:key];
        
        STOREKIT_LOG_2("Process transaction with key: %s\n", [key UTF8String]);
        
        [self processTransaction:transaction];
    }
    
    StoreKit_ReceiveTransactionsEndCallback(externalData);
}

-(void)processTransaction:(SKPaymentTransaction*)transaction
{
    NSString* state = @"";
    NSString* transactionID = @"transDummy";
    
    switch (transaction.transactionState)
    {
        case SKPaymentTransactionStatePurchased:
            STOREKIT_LOG_1("Process transaction status purchased.\n");
            
            state = @"purchased";
            transactionID = transaction.transactionIdentifier;
            break;
        
        case SKPaymentTransactionStateFailed:
            if (transaction.error.code == SKErrorPaymentCancelled)
            {
                STOREKIT_LOG_2("Process transaction status cancelled, error=%d.\n", (int) transaction.error.code);
            
                state = @"cancelled";
            }
            else
            {
                STOREKIT_LOG_2("Process transaction status failed, error=%d.\n", (int) transaction.error.code);
            
                state = @"failed";
            }
            break;
        
        case SKPaymentTransactionStateRestored:
            STOREKIT_LOG_1("Process transaction status restored.\n");
            
            state = @"restored";
            transactionID = transaction.transactionIdentifier;
            break;
            
        case SKPaymentTransactionStateDeferred:
            STOREKIT_LOG_1("Process transaction status deferred.\n");
            
            state = @"processing";
            break;
            
        case SKPaymentTransactionStatePurchasing:
            STOREKIT_LOG_1("Process transaction status purchasing.\n");
            
            state = @"processing";
            break;
    }
    
    
    
    StoreKit_AddTransactionCallback([transactionID UTF8String], [transaction.payment.productIdentifier UTF8String], [state UTF8String], externalData);
}

-(void)validateProducts:(const char*)ids
{
    NSString* strIdList = [NSString stringWithCString:ids encoding:NSUTF8StringEncoding];
    NSSet* idSet = [NSSet setWithArray:[strIdList componentsSeparatedByString:@";"]];
    productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:idSet];
    productsRequest.delegate = self;
    [productsRequest start];
}

-(void)purchase:(const char*)prodId
{
    SKPayment* payment = [SKPayment paymentWithProductIdentifier:[NSString stringWithCString:prodId encoding:NSUTF8StringEncoding]];
    
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

-(void)restoreTransactions
{
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    StoreKit_RestoreFinishedCallback(externalData, 0);
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    StoreKit_RestoreFinishedCallback(externalData, 1);
}


-(BOOL)finishTransaction:(const char*)transId
{
    NSString* strId = [NSString stringWithCString:transId encoding:NSUTF8StringEncoding];
    
    SKPaymentTransaction* trans = [dictTransactions objectForKey:strId];
    if (trans != nil)
    {
        STOREKIT_LOG_2("Finishing transaction with id=%s.\n", transId);
        
        [[SKPaymentQueue defaultQueue] finishTransaction: trans];
        [dictTransactions removeObjectForKey:strId];
        return YES;
    }
    else
    {
        STOREKIT_LOG_2("Transaction with id=%s not found!\n", transId);
        return NO;
    }
}

@end




@implementation SKProduct (LocalizedPrice)

- (NSString* )localizedPrice
{
    NSNumberFormatter* numberFormatter = [[NSNumberFormatter alloc] init];
    [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    [numberFormatter setLocale:self.priceLocale];
    NSString* formattedString = [numberFormatter stringFromNumber:self.price];
    [numberFormatter release];
    return formattedString;
}

@end
