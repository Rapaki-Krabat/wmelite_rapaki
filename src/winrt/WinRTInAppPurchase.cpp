
#include <string.h>

#include "WinRTInAppPurchase.h"

#include "SDL_log.h"

#include <ppltasks.h>

using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace concurrency;
using namespace Windows::Foundation;

static int lastLicenseCheckStartTime = 0;

static int licenseCheckResult = -1;

// license information
// https://github.com/Microsoft/Windows-universal-samples/blob/win10-1507/Samples/Store/cpp/Scenario1_TrialMode.xaml.cpp

// async task
// https://msdn.microsoft.com/de-de/library/hh750082.aspx

// request receipt
// https://msdn.microsoft.com/library/windows/apps/hh967811

// MS Store samples
// https://github.com/Microsoft/Windows-universal-samples/tree/master/Samples/Store

class WinRTInAppPurchase
{
public:
	void WinRTLicCheckStartAsync();

	void WinRTLicCheckCancel();

	int WinRTLicCheckStatus();

	// void WinRTLicCheckCompletionHandler(IAsyncOperation<String^> ^ asyncOp, AsyncStatus asyncStatus);

private:
	IAsyncOperation<String^> ^ appReceipt = nullptr;

};

static WinRTInAppPurchase *instance = new WinRTInAppPurchase();

void LicenseCheckStartAsync(int startTime)
{
	lastLicenseCheckStartTime = startTime;

	licenseCheckResult = -1;

	instance->WinRTLicCheckStartAsync();
}

int LicenseCheckStartTime()
{
	return lastLicenseCheckStartTime;
}

// 0 --> not pending, != 0 --> pending
int LicenseCheckPollStatus()
{
	return instance->WinRTLicCheckStatus();
}

void LicenseCheckAbort()
{
	instance->WinRTLicCheckCancel();
	lastLicenseCheckStartTime = 0;
	licenseCheckResult        = -1;
}

// 0 --> check passed, > 0 --> check failed, < 0 --> inconclusive (not possible to check)
int LicenseCheckResultStatus()
{
	return licenseCheckResult;
}

void LicenseCheckResultDetail(char *str, int maxlen)
{
	if (licenseCheckResult == -1)
	{
		strcpy(str, "License not checked yet!");
	}
	else if (licenseCheckResult == 0)
	{
		strcpy(str, "Licensed.");
	}
	else
	{
		strcpy(str, "Unlicensed.");
	}
}

void WinRTInAppPurchase::WinRTLicCheckStartAsync()
{
	appReceipt = CurrentApp::GetAppReceiptAsync();
	// appReceipt->Completed = ref new AsyncOperationCompletedHandler<String ^>(this, &WinRTLicCheck::WinRTLicCheckCompletionHandler);

	create_task(appReceipt).then([this](String^ resultStr){
		const unsigned int resultLen = resultStr->Length();
		const wchar_t* wide_chars = resultStr->Data();
		char chars[4096];
		memset(chars, 0, 4096);
		wcstombs(chars, wide_chars, 4094);

		SDL_Log("len=%d, val=%s\n", resultLen, chars);

		licenseCheckResult = 0;
	}
	);
#if 0
	create_async([]
	{
		auto licenseInformation = CurrentApp::LicenseInformation;
		if (licenseInformation->IsActive)
		{
			licenseCheckResult = 1;
		}
		else
		{
			licenseCheckResult = -1;
		}
	});
#endif
}

void WinRTInAppPurchase::WinRTLicCheckCancel()
{
	if (appReceipt != nullptr)
	{
		appReceipt->Cancel();
	}
}

int WinRTInAppPurchase::WinRTLicCheckStatus()
{
	if (appReceipt != nullptr)
	{
		if (appReceipt->Status == AsyncStatus::Completed)
		{
			return 0;
		}
	}

	return 1;
}
