
#include <string.h>

#include "StoreKit_functions.h"
#include "StoreKit_callbacks.h"

// definitions for stringutil declarations
typedef unsigned char       BYTE; 
typedef unsigned long       DWORD;

#include "dctypes.h"

#include "StringUtil.h"

#include "SDL_log.h"

#include <ppltasks.h>

using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

static void *callbackObj;

// async task
// https://msdn.microsoft.com/de-de/library/hh750082.aspx

// MS Store samples
// https://github.com/Microsoft/Windows-universal-samples/tree/master/Samples/Store

class WinRTInAppPurchase
{
public:
	void WinRTInAppListItems(const char* prodIdList);

	void WinRTInAppPurchaseItem(const char* productId);

	void WinRTInAppRestorePurchasedItems();

	void WinRTInAppConvertRTString(String^ winrtString, char* cString, int maxLen);

	Platform::String^ WinRTInAppConvertCString(const char* cString);

	void WinRTInAppUpdateEncoding(TTextEncoding newEncoding);

private:
	IAsyncOperation<ListingInformation^> ^ listInfo = nullptr;

	char prodIdListCopy[4096];

	IAsyncOperation<ListingInformation^> ^ restoreListInfo = nullptr;

	IAsyncOperation<PurchaseResults^>^ purchaseResult = nullptr;

	String^ productIDCopy;

	TTextEncoding currEncoding = TEXT_ANSI;
};

static WinRTInAppPurchase *instance = new WinRTInAppPurchase();

void StoreKit_SetExternalData(void* data)
{
	callbackObj = data;
}

int StoreKit_IsStoreAvailable()
{
    // Windows Store is always available
    return 1;
}

void StoreKit_EnableEvents()
{
    // dummy
}

void StoreKit_DisableEvents()
{
    // dummy
}

void StoreKit_ValidateProducts(const char* ids, TTextEncoding encoding)
{
	instance->WinRTInAppUpdateEncoding(encoding);
	instance->WinRTInAppListItems(ids);
}

void StoreKit_Purchase(const char* prodId, TTextEncoding encoding)
{
	instance->WinRTInAppUpdateEncoding(encoding);
	instance->WinRTInAppPurchaseItem(prodId);
}

void StoreKit_RestoreTransactions(TTextEncoding encoding)
{
	instance->WinRTInAppUpdateEncoding(encoding);
	instance->WinRTInAppRestorePurchasedItems();
}

int StoreKit_FinishTransaction(const char* transId, TTextEncoding encoding)
{
	instance->WinRTInAppUpdateEncoding(encoding);
	
	// dummy
	return 1;
}

void WinRTInAppPurchase::WinRTInAppListItems(const char *prodIdList)
{
	prodIdListCopy[4095] = 0;
	strncpy(prodIdListCopy, prodIdList, 4095);
	
	listInfo = CurrentApp::LoadListingInformationAsync();

	create_task(listInfo).then([this](ListingInformation^ info) {
		
		StoreKit_ReceiveProductsStartCallback(callbackObj);

		int start = 0;
		int i = 0;
		int end = (int) strlen(prodIdListCopy);
		char onekey[4096];

		SDL_Log("Product keys to search: %s.\n", prodIdListCopy);

		while (i < end)
		{
			while ((i < end) && (prodIdListCopy[i] != ';'))
			{
				i++;
			}

			memset(onekey, 0, 4096);
			if (i > start)
			{
				memcpy(onekey, &prodIdListCopy[start], (i - start));

				SDL_Log("One product key to search: %s.\n", onekey);

				String^ rtKey = WinRTInAppConvertCString(onekey);

				try
				{

					if (info->ProductListings->HasKey(rtKey))
					{
						ProductListing^ item = info->ProductListings->Lookup(rtKey);

						char name[4096];
						char desc[4096];
						char price[4096];

						WinRTInAppConvertRTString(item->Name, name, 4096);
						WinRTInAppConvertRTString(item->Description, desc, 4096);
						WinRTInAppConvertRTString(item->FormattedPrice, price, 4096);

						SDL_Log("Valid product: id='%s' name='%s' description='%s' price='%s'\n",
							onekey, name, desc, price);

						StoreKit_AddValidProductCallback(onekey, name, desc, price, callbackObj);
					}
					else
					{
						SDL_Log("Invalid product: id='%s'\n", onekey);

						StoreKit_AddInvalidProductCallback(onekey, callbackObj);
					}
				}
				catch (Platform::Exception^ exception)
				{
					char chars[4096];
					WinRTInAppConvertRTString(exception->ToString(), chars, 4096);

					SDL_Log("Error fetching store listing: %s.\n", chars);
				}
			}

			i++;
			start = i;
		}

		// debug: print what we actually found

#if 1

		unsigned int productListSize;

		productListSize = info->ProductListings->Size;

		SDL_Log("List of in app items found size: %u\n", productListSize);

		for (IIterator<IKeyValuePair<String^, ProductListing^>^>^ iter = info->ProductListings->First(); iter->HasCurrent; iter->MoveNext())
		{
			try
			{

				String^ id = iter->Current->Key;

				char idCString[4096];

				WinRTInAppConvertRTString(id, idCString, 4096);

				SDL_Log("Item id=%s found.\n", idCString);
			}
			catch (Platform::Exception^ exception)
			{
				char chars[4096];
				WinRTInAppConvertRTString(exception->ToString(), chars, 4096);

				SDL_Log("Error iterating store listing: %s.\n", chars);
			}
		}

#endif

		StoreKit_ReceiveProductsEndCallback(callbackObj);
	}
	).then([this](task<void> t)
	{
		try {
			t.get();
			// .get() didn't throw, so we succeeded.
		}
		catch (Platform::Exception^ exception) {
			// handle error
			char exChars[4096];
			WinRTInAppConvertRTString(exception->ToString(), exChars, 4096);

			SDL_Log("Error thrown from task iterating store listing: %s.\n", exChars);
		}

	});
}

void WinRTInAppPurchase::WinRTInAppPurchaseItem(const char* productId)
{
	productIDCopy = WinRTInAppConvertCString(productId);

	purchaseResult = CurrentApp::RequestProductPurchaseAsync(productIDCopy);

	create_task(purchaseResult).then([this](PurchaseResults^ res) {
		
		// do not call transaction_start() because only the purchased product will be added
		// and the callback would erase the whole list of (restored) transactions
		//
		// leave that to the RestorePurchasedItems() method
		
		try {
			char idCString[4096];
			char transIDCString[4096];

			WinRTInAppConvertRTString(productIDCopy->ToString(), idCString, 4096);

			transIDCString[0] = 0;
			strcat(transIDCString, "trID_");
			strcat(transIDCString, idCString);

			auto licenseInformation = CurrentApp::LicenseInformation;
			if (licenseInformation->ProductLicenses->Lookup(productIDCopy)->IsActive)
			{
				SDL_Log("Purchasing %s successful!\n", idCString);
				
				StoreKit_AddTransactionCallback(transIDCString, idCString, "purchased", callbackObj);
			}
			else
			{
				SDL_Log("Purchasing %s FAILED!\n", idCString);

				StoreKit_AddTransactionCallback(transIDCString, idCString, "failed", callbackObj);
			}
		}
		catch (Platform::Exception^ exception)
		{
			char chars[4096];
			char idCString[4096];
			char transIDCString[4096];

			WinRTInAppConvertRTString(exception->ToString(), chars, 4096);
			WinRTInAppConvertRTString(productIDCopy->ToString(), idCString, 4096);

			transIDCString[0] = 0;
			strcat(transIDCString, "trID_");
			strcat(transIDCString, idCString);

			SDL_Log("Error purchasing item: %s.\n", chars);

			StoreKit_AddTransactionCallback(transIDCString, idCString, "failed", callbackObj);
		}

		StoreKit_ReceiveTransactionsEndCallback(callbackObj);
	}
	).then([this](task<void> t)
	{
		try {
			t.get();
			// .get() didn't throw, so we succeeded.
		}
		catch (Platform::Exception^ exception) {
			// handle error
			char exChars[4096];
			WinRTInAppConvertRTString(exception->ToString(), exChars, 4096);

			SDL_Log("Error thrown from task purchasing an item: %s.\n", exChars);
		}

	});
}

void WinRTInAppPurchase::WinRTInAppRestorePurchasedItems()
{
	restoreListInfo = CurrentApp::LoadListingInformationAsync();

	create_task(restoreListInfo).then([this](ListingInformation^ info) {

		unsigned int productListSize;
		
		bool success = true;

		StoreKit_ReceiveTransactionsStartCallback(callbackObj);

		productListSize = info->ProductListings->Size;

		SDL_Log("List of in app items size: %u\n", productListSize);

		auto licenseInformation = CurrentApp::LicenseInformation;

		for (IIterator<IKeyValuePair<String^, ProductListing^>^>^ iter = info->ProductListings->First(); iter->HasCurrent; iter->MoveNext())
		{
			try
			{

				String^ id = iter->Current->Key;

				auto productLicense = licenseInformation->ProductLicenses->Lookup(id);

				char idCString[4096];

				WinRTInAppConvertRTString(id, idCString, 4096);

				char transIDCString[4096];
				transIDCString[0] = 0;
				strcat(transIDCString, "trID_");
				strcat(transIDCString, idCString);

				if (productLicense->IsActive)
				{
					SDL_Log("Item id=%s was previously purchased.\n", idCString);

					StoreKit_AddTransactionCallback(transIDCString, idCString, "restored", callbackObj);
				}
				else
				{
					SDL_Log("Item id=%s was NOT previously purchased.\n", idCString);

					StoreKit_AddTransactionCallback(transIDCString, idCString, "failed", callbackObj);
				}
			}
			catch (Platform::Exception^ exception)
			{
				char chars[4096];
				WinRTInAppConvertRTString(exception->ToString(), chars, 4096);

				SDL_Log("Error fetching store listing: %s.\n", chars);

				success = false;
			}
		}

		StoreKit_ReceiveTransactionsEndCallback(callbackObj);

		StoreKit_RestoreFinishedCallback(callbackObj, (success == true) ? 0 : 1);
	}
	).then([this](task<void> t)
	{
		try {
			t.get();
			// .get() didn't throw, so we succeeded.
		}
		catch (Platform::Exception^ exception) {
			// handle error
			char exChars[4096];
			WinRTInAppConvertRTString(exception->ToString(), exChars, 4096);

			SDL_Log("Error thrown from task trying to restore purchased items: %s.\n", exChars);
		}

	});
}

void WinRTInAppPurchase::WinRTInAppConvertRTString(String^ winrtString, char* cString, int maxLen)
{
	const unsigned int resultLen = winrtString->Length();
	const wchar_t* wide_chars = winrtString->Data();
	memset(cString, 0, maxLen);
	if (currEncoding == TEXT_ANSI)
	{
		// convert to current locale and hope for the best
		wcstombs(cString, wide_chars, (maxLen - 1));
	}
	else
	{
		// convert to UTF-8 with hopefully no loss
		Utf8String tmpStr;
		size_t tmpLength;
		
		tmpStr = StringUtil::WideToUtf8(wide_chars);
		tmpLength = maxLen;
		if (tmpStr.length() < maxLen)
		{
			tmpLength = tmpStr.length();
		}

		memcpy(cString, tmpStr.c_str(), tmpLength);
	}
}

Platform::String^ WinRTInAppPurchase::WinRTInAppConvertCString(const char* cString)
{
	std::string s_str;
	std::wstring wid_str;

	s_str = std::string(cString);

	if (currEncoding == TEXT_ANSI)
	{
		// simply transfer to wide string
		wid_str = std::wstring(s_str.begin(), s_str.end());
	}
	else
	{
		// convert UTF-8 to wide string
		wid_str = StringUtil::Utf8ToWide(s_str);
	}

	const wchar_t* w_char = wid_str.c_str();
	Platform::String^ p_string = ref new Platform::String(w_char);

	return p_string;
}

void WinRTInAppPurchase::WinRTInAppUpdateEncoding(TTextEncoding newEncoding)
{
	currEncoding = newEncoding;
}
