
#include <string.h>

#include "WinRTInAppPurchase.h"
#include "WinRTInAppCallbacks.h"

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

private:
	IAsyncOperation<ListingInformation^> ^ listInfo = nullptr;

	IAsyncOperation<ListingInformation^> ^ restoreListInfo = nullptr;

	IAsyncOperation<PurchaseResults^>^ purchaseResult = nullptr;
};

static WinRTInAppPurchase *instance = new WinRTInAppPurchase();

void wrtiap_SetCallbackObject(void *obj)
{
	callbackObj = obj;
}

void wrtiap_SetEventsEnabledDummy(bool enabled)
{

}

void wrtiap_VerifyInAppProductsList(const char* prodIdList)
{
	instance->WinRTInAppListItems(prodIdList);
}

void wrtiap_StartPurchase(const char* productId)
{
	instance->WinRTInAppPurchaseItem(productId);
}

int wrtiap_ConfirmPurchaseDummy(const char* transId)
{
	return 1;
}

void wrtiap_RefreshPreviousPurchases()
{
	instance->WinRTInAppRestorePurchasedItems();
}

void WinRTInAppPurchase::WinRTInAppListItems(const char *prodIdList)
{
	listInfo = CurrentApp::LoadListingInformationAsync();

	create_task(listInfo).then([this,prodIdList](ListingInformation^ info) {
		
		WinRTInAppPurchase_ReceiveProductsStartCallback(callbackObj);

		int start = 0;
		int i = 0;
		int end = strlen(prodIdList);
		char onekey[4096];

		SDL_Log("Product keys to search: %s.\n", prodIdList);

		while (i < end)
		{
			while ((i < end) && (prodIdList[i] != ';'))
			{
				i++;
			}

			memset(onekey, 0, 4096);
			if (i > start)
			{
				memcpy(onekey, &prodIdList[start], (i - start));

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

						WinRTInAppPurchase_AddValidProductCallback(onekey, name, desc, price, callbackObj);
					}
					else
					{
						WinRTInAppPurchase_AddInvalidProductCallback(onekey, callbackObj);
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

		WinRTInAppPurchase_ReceiveProductsEndCallback(callbackObj);
	}
	);
}

void WinRTInAppPurchase::WinRTInAppPurchaseItem(const char* productId)
{
	String^ rtId = WinRTInAppConvertCString(productId);

	purchaseResult = CurrentApp::RequestProductPurchaseAsync(rtId);

	create_task(purchaseResult).then([this,rtId](PurchaseResults^ res) {
		
		// do not call transaction_start() because only the purchased product will be added
		// and the callback would erase the whole list of (restored) transactions
		//
		// leave that to the RestorePurchasedItems() method
		
		try {
			char idCString[4096];
			char transIDCString[4096];

			WinRTInAppConvertRTString(rtId->ToString(), idCString, 4096);

			transIDCString[0] = 0;
			strcat(transIDCString, "trID_");
			strcat(transIDCString, idCString);

			auto licenseInformation = CurrentApp::LicenseInformation;
			if (licenseInformation->ProductLicenses->Lookup(rtId)->IsActive)
			{
				SDL_Log("Purchasing %s successful!\n", idCString);
				
				WinRTInAppPurchase_AddTransactionCallback(transIDCString, idCString, "purchased", callbackObj);
			}
			else
			{
				SDL_Log("Purchasing %s FAILED!\n", idCString);

				WinRTInAppPurchase_AddTransactionCallback(transIDCString, idCString, "failed", callbackObj);
			}
		}
		catch (Platform::Exception^ exception)
		{
			char chars[4096];
			char idCString[4096];
			char transIDCString[4096];

			WinRTInAppConvertRTString(exception->ToString(), chars, 4096);
			WinRTInAppConvertRTString(rtId->ToString(), idCString, 4096);

			transIDCString[0] = 0;
			strcat(transIDCString, "trID_");
			strcat(transIDCString, idCString);

			SDL_Log("Error purchasing item: %s.\n", chars);

			WinRTInAppPurchase_AddTransactionCallback(transIDCString, idCString, "failed", callbackObj);
		}

		WinRTInAppPurchase_ReceiveTransactionsEndCallback(callbackObj);
	}
	);
}

void WinRTInAppPurchase::WinRTInAppRestorePurchasedItems()
{
	restoreListInfo = CurrentApp::LoadListingInformationAsync();

	create_task(restoreListInfo).then([this](ListingInformation^ info) {

		bool success = true;

		WinRTInAppPurchase_ReceiveTransactionsStartCallback(callbackObj);

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

					WinRTInAppPurchase_AddTransactionCallback(transIDCString, idCString, "restored", callbackObj);
				}
				else
				{
					SDL_Log("Item id=%s was NOT previously purchased.\n", idCString);

					WinRTInAppPurchase_AddTransactionCallback(transIDCString, idCString, "failed", callbackObj);
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

		WinRTInAppPurchase_ReceiveTransactionsEndCallback(callbackObj);

		WinRTInAppPurchase_RestoreFinishedCallback(callbackObj, (success == true) ? 0 : 1);
	}
	);
}

void WinRTInAppPurchase::WinRTInAppConvertRTString(String^ winrtString, char* cString, int maxLen)
{
	const unsigned int resultLen = winrtString->Length();
	const wchar_t* wide_chars = winrtString->Data();
	memset(cString, 0, maxLen);
	wcstombs(cString, wide_chars, (maxLen - 1));
}

Platform::String^ WinRTInAppPurchase::WinRTInAppConvertCString(const char* cString)
{
	std::string s_str = std::string(cString);
	std::wstring wid_str = std::wstring(s_str.begin(), s_str.end());
	const wchar_t* w_char = wid_str.c_str();
	Platform::String^ p_string = ref new Platform::String(w_char);

	return p_string;
}
