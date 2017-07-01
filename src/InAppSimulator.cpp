/*
 * InAppSimulator.cpp
 *
 *  Created on: 30.06.2017
 *      Author: daniel
 */

#include "StoreKit_functions.h"
#include "StoreKit_callbacks.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __WIN32__
#    include <windows.h>
#else
#    include <pthread.h>
#    include <time.h>
#endif

static void *callbackObj;

static bool storeAvailable;

typedef struct
{
	char id[200];
	char name[200];
	bool valid;
	char desc[500];
	char price[50];
	bool purchased;
	int  purchaseResult;
} storeitem;

static int validateDelayMs;
static int restoreDelayMs;
static int purchaseDelayMs;

static bool restoreResult;
static bool validateResult;

static char purchaseIDCopy[4096];
static char validateIDCopy[4096];

static storeitem items[10];
int maxItems;

////////////////////////////////////////////////////////////////////////////////
void StoreKit_SetExternalData(void* data)
{
	callbackObj = data;

	// initialize defaults
	storeAvailable = false;
	maxItems = 0;
	
	validateDelayMs = 2000;
    restoreDelayMs  = 2000;
    purchaseDelayMs = 4000;

    restoreResult = true;
    validateResult = true;

	// load simualtor data from file
	FILE* f = fopen("storesimulator.txt", "r");

	if (f != NULL)
	{
		printf("Store config file found!\n");

		char line[500];
		while (fgets(line, 500, f) != NULL)
		{
		    // printf(line);

		    if ((strncmp(line, "#", 1) == 0) || (strncmp(line, "//", 2) == 0))
		    {
		    	// printf("Skipping comment line!\n");
		    }
		    else
		    {
				// check if store is available at all
				if (strncmp(line, "storeavailable:", 15) == 0)
				{
					// printf("Store available line!\n");
					if (strncasecmp(line + 15, "true", 4) == 0)
					{
						printf("Store is available!\n");
						storeAvailable = true;
					}
				}
				
				// check if restore action shall fail
				if (strncmp(line, "restore:", 8) == 0)
				{
					if (strncasecmp(line + 8, "false", 5) == 0)
					{
						printf("Restore action will fail!\n");
						restoreResult = false;
					}
				}
				
				// check if validate action shall fail
				if (strncmp(line, "validate:", 9) == 0)
				{
					if (strncasecmp(line + 9, "false", 5) == 0)
					{
						printf("Validate action will fail!\n");
						validateResult = false;
					}
				}
				
				// parse validate delay
				if (strncmp(line, "valp:", 5) == 0)
				{
				  validateDelayMs  = atoi(line + 5);
				}

				// parse restore delay
				if (strncmp(line, "rest:", 5) == 0)
				{
				  restoreDelayMs  = atoi(line + 5);
				}

				// parse purchase delay
				if (strncmp(line, "purc:", 5) == 0)
				{
				  purchaseDelayMs  = atoi(line + 5);
				}

				// check for items in the store
				if (strncmp(line, "item:", 5) == 0)
				{
					char* start = NULL;
					char* end = NULL;
					char tmp[500];

					memset(items[maxItems].id, 0, 200);
					memset(items[maxItems].name, 0, 200);
					memset(items[maxItems].desc, 0, 200);
					memset(items[maxItems].price, 0, 200);
					items[maxItems].valid = false;
					items[maxItems].purchased = false;
					items[maxItems].purchaseResult = 0;

					// search id (terminated at ",")
					start = strstr(line + 5, "id=");
					if (start != NULL) end = strchr(start, 44);

					if ((start != NULL) && (end != NULL))
					{
						// printf("Id string starts at %d and ends at %d len=%d!\n", (int) (start + 3 - line), (int) (end - line), (int) (end - start - 3));
						strncpy(items[maxItems].id, start + 3, (int) (end - start - 3));

						// printf("Parsed id='%s'.\n", items[maxItems].id);

						// search name (terminated at ",")
						start = strstr(line + 5, "name=");
						end = NULL;
						if (start != NULL) end = strchr(start, 44);

						if ((start != NULL) && (end != NULL))
						{
							strncpy(items[maxItems].name, start + 5, (int) (end - start - 5));
							items[maxItems].valid = true;
						}

						// only if a name is found the item is valid and other props can be searched for
						if (items[maxItems].valid == true)
						{
							// search description (terminated at ",")
							start = strstr(line + 5, "desc=");
							end = NULL;
							if (start != NULL) end = strchr(start, 44);

							if ((start != NULL) && (end != NULL))
							{
								strncpy(items[maxItems].desc, start + 5, (int) (end - start - 5));
							}

							// search price (terminated at ",")
							start = strstr(line + 5, "price=");
							end = NULL;
							if (start != NULL) end = strchr(start, 44);

							if ((start != NULL) && (end != NULL))
							{
								strncpy(items[maxItems].price, start + 6, (int) (end - start - 6));
							}

							// search purchased (terminated at ",")
							start = strstr(line + 5, "purchased=");
							end = NULL;
							if (start != NULL) end = strchr(start, 44);

							if ((start != NULL) && (end != NULL))
							{
								memset(tmp, 0, 500);
								strncpy(tmp, start + 10, (int) (end - start - 10));
								if (strncasecmp(tmp, "true", 4) == 0)
								{
									items[maxItems].purchased = true;
								}
							}

							// search result (terminated at ",")
							start = strstr(line + 5, "result=");
							end = NULL;
							if (start != NULL) end = strchr(start, 44);

							if ((start != NULL) && (end != NULL))
							{
								memset(tmp, 0, 500);
								strncpy(tmp, start + 7, (int) (end - start - 7));
								items[maxItems].purchaseResult = atoi(tmp);
							}
						}
					}

					printf("Item found. Id='%s' Name='%s' Valid='%s' Description='%s' Price='%s' Purchased='%s' Purchaseresult='%d'.\n", items[maxItems].id, items[maxItems].name, (items[maxItems].valid == true) ? "true" : "false", items[maxItems].desc, items[maxItems].price, (items[maxItems].purchased == true) ? "true" : "false", items[maxItems].purchaseResult);

					// found an item, so increase number of items
					maxItems++;
				}
		    }
		}

		fclose(f);
	}
	else
	{
		printf("Store config file NOT found!\n");
	}
}

////////////////////////////////////////////////////////////////////////////////
int StoreKit_IsStoreAvailable()
{
	return storeAvailable;
}

////////////////////////////////////////////////////////////////////////////////
void StoreKit_EnableEvents()
{
    // dummy
}

////////////////////////////////////////////////////////////////////////////////
void StoreKit_DisableEvents()
{
    // dummy
}

////////////////////////////////////////////////////////////////////////////////
#ifdef __WIN32__
static DWORD WINAPI ValidateProductsThread(LPVOID arg)
#else
void* ValidateProductsThread(void *arg)
#endif
{
    const char *ids = (const char *) arg;
    
	int start = 0;
	int i = 0;
	int end = (int) strlen(ids);
	char onekey[4096];
	int k;

#ifdef __WIN32__
    Sleep(validateDelayMs);
#else
    usleep(validateDelayMs * 1000);
#endif

	StoreKit_ReceiveProductsStartCallback(callbackObj);

	// do not issue any callbacks for products if the validation itself shall fail
	while ((i < end) && (validateResult == true))
	{
		while ((i < end) && (ids[i] != ';'))
		{
			i++;
		}

		memset(onekey, 0, 4096);
		if (i > start)
		{
			memcpy(onekey, &ids[start], (i - start));

			// printf("One product key to search: %s.\n", onekey);

			for (k = 0; k < maxItems; k++)
			{
			    // check length before checking content
			    if (strlen(onekey) == strlen(items[k].id))
			    {
                    if (strncmp(onekey, items[k].id, strlen(items[k].id)) == 0)
                    {
                        break;
                    }
                }
			}

			if (k < maxItems)
			{
			    if (items[k].valid == true)
			    {
			        // valid product
			        StoreKit_AddValidProductCallback(onekey, items[k].name, items[k].desc, items[k].price, callbackObj);
			    }
			    else
			    {
                    // existing invalid product
                    StoreKit_AddInvalidProductCallback(onekey, callbackObj);
			    }
			}
			else
			{
				// non-existing invalid product
				StoreKit_AddInvalidProductCallback(onekey, callbackObj);
			}
		}

		i++;
		start = i;
	}

	StoreKit_ReceiveProductsEndCallback(callbackObj);
    
#ifdef __WIN32__
    return 0;
#else
    return (void *) 0;
#endif    
}

////////////////////////////////////////////////////////////////////////////////
void StoreKit_ValidateProducts(const char* ids)
{
    // must copy stack variable to local memory
    strcpy(validateIDCopy, ids);
    
#ifdef __WIN32__
    CreateThread(NULL, 0, ValidateProductsThread, (LPVOID) validateIDCopy, 0, NULL);
#else
    pthread_d threadId;
    pthread_create(&threadId, NULL, ValidateProductsThread, (void *) validateIDCopy);
#endif
}

////////////////////////////////////////////////////////////////////////////////
#ifdef __WIN32__
static DWORD WINAPI PurchaseProductsThread(LPVOID arg)
#else
void* PurchaseProductsThread(void *arg)
#endif
{
    const char *prodId = (const char *) arg;

	int k;
	char transId[4096];
	bool found = false;
    
#ifdef __WIN32__
    Sleep(purchaseDelayMs);
#else
    usleep(purchaseDelayMs * 1000);
#endif

	for (k = 0; k < maxItems; k++)
	{
        // check length before checking content
        if (strlen(prodId) == strlen(items[k].id))
        {
            if (strncmp(prodId, items[k].id, strlen(items[k].id)) == 0)
            {
                found = true;
    
                transId[0] = 0;
                strcat(transId, "purTrID_");
                strcat(transId, items[k].id);
    
                switch(items[k].purchaseResult)
                {
                case 0:
                    // successful purchase --> change database so that re-purchase is not possible
                    items[k].purchased = true;
                    items[k].purchaseResult = 2;
                    StoreKit_AddTransactionCallback(transId, items[k].id, "purchased", callbackObj);
                    break;
                case 1:
                    StoreKit_AddTransactionCallback(transId, items[k].id, "cancelled", callbackObj);
                    break;
                default:
                    StoreKit_AddTransactionCallback(transId, prodId, "failed", callbackObj);
                }
    
                break;
            }
        }
	}

	// not found --> failed
	if (found == false)
	{
		transId[0] = 0;
		strcat(transId, "purTrID_");
		strcat(transId, prodId);

		StoreKit_AddTransactionCallback(transId, prodId, "failed", callbackObj);
	}

	StoreKit_ReceiveTransactionsEndCallback(callbackObj);

#ifdef __WIN32__
    return 0;
#else
    return (void *) 0;
#endif    
}

////////////////////////////////////////////////////////////////////////////////
void StoreKit_Purchase(const char* prodId)
{
    // must copy stack variable to local memory
    strcpy(purchaseIDCopy, prodId);
    
#ifdef __WIN32__
    CreateThread(NULL, 0, PurchaseProductsThread, (LPVOID) purchaseIDCopy, 0, NULL);
#else
    pthread_d threadId;
    pthread_create(&threadId, NULL, PurchaseProductsThread, (void *) purchaseIDCopy);
#endif
}

////////////////////////////////////////////////////////////////////////////////
#ifdef __WIN32__
static DWORD WINAPI RestoreTransactionsThread(LPVOID arg)
#else
void* RestoreTransactionsThread(void *arg)
#endif
{
	int k;
	char transId[4096];

#ifdef __WIN32__
    Sleep(restoreDelayMs);
#else
    usleep(restoreDelayMs * 1000);
#endif

	StoreKit_ReceiveTransactionsStartCallback(callbackObj);

    if (restoreResult == true)	
	{
        for (k = 0; k < maxItems; k++)
        {
            transId[0] = 0;
            strcat(transId, "trID_");
            strcat(transId, items[k].id);
    
            if (items[k].purchased == true)
            {
                StoreKit_AddTransactionCallback(transId, items[k].id, "restored", callbackObj);
            }
            else
            {
                StoreKit_AddTransactionCallback(transId, items[k].id, "failed", callbackObj);
            }
        }
    }

	StoreKit_ReceiveTransactionsEndCallback(callbackObj);

	StoreKit_RestoreFinishedCallback(callbackObj, (restoreResult == true) ? 0 : 1);
    
#ifdef __WIN32__
    return 0;
#else
    return (void *) 0;
#endif    
}

////////////////////////////////////////////////////////////////////////////////
void StoreKit_RestoreTransactions()
{
#ifdef __WIN32__
    CreateThread(NULL, 0, RestoreTransactionsThread, (LPVOID) NULL, 0, NULL);
#else
    pthread_d threadId;
    pthread_create(&threadId, NULL, RestoreTransactionsThread, (void *) NULL);
#endif
}

////////////////////////////////////////////////////////////////////////////////
int StoreKit_FinishTransaction(const char* transId)
{
    // dummy
	return 1;
}

