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
} storeitem;

static storeitem items[10];
int maxItems;

void StoreKit_SetExternalData(void* data)
{
	callbackObj = data;

	// initialize defaults
	storeAvailable = false;
	maxItems = 0;

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


						}
					}

					printf("Item found. Id='%s' Name='%s' Valid='%s' Description='%s' Price='%s' Purchased='%s'.\n", items[maxItems].id, items[maxItems].name, (items[maxItems].valid == true) ? "true" : "false", items[maxItems].desc, items[maxItems].price, (items[maxItems].purchased == true) ? "true" : "false");

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

int StoreKit_IsStoreAvailable()
{
	return storeAvailable;
}

void StoreKit_EnableEvents()
{
    // dummy
}

void StoreKit_DisableEvents()
{
    // dummy
}

void StoreKit_ValidateProducts(const char* ids)
{

}

void StoreKit_Purchase(const char* prodId)
{

}

void StoreKit_RestoreTransactions()
{

}

int StoreKit_FinishTransaction(const char* transId)
{
    // dummy
	return 1;
}
