
#include <string.h>

#include "LicenseCheck.h"

#include "SDL_log.h"

#include <ppltasks.h>

using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace concurrency;
using namespace Windows::Foundation;

static int lastLicenseCheckStartTime = 0;

static int licenseCheckResult = 0;

// license information
// https://github.com/Microsoft/Windows-universal-samples/blob/win10-1507/Samples/Store/cpp/Scenario1_TrialMode.xaml.cpp

// async task
// https://msdn.microsoft.com/de-de/library/hh750082.aspx

// request receipt
// https://msdn.microsoft.com/library/windows/apps/hh967811

static IAsyncOperation<String^> ^ appReceipt;

void LicenseCheckStartAsync(int startTime)
{
	lastLicenseCheckStartTime = startTime;

	licenseCheckResult = 0;

	appReceipt = CurrentApp::GetAppReceiptAsync();
	appReceipt->Completed = ref new AsyncOperationCompletedHandler<String ^>(this, AppReceiptCompletionHandler);

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

void AppReceiptCompletionHandler(IAsyncOperation<String^> ^ asyncOp, AsyncStatus asyncStatus)
{

}

int LicenseCheckStartTime()
{
	return lastLicenseCheckStartTime;
}

// 0 --> not pending, != 0 --> pending
int LicenseCheckPollStatus()
{
	if (appReceipt->Completed())
	return (licenseCheckResult == 0) ? 1 : 0;
}

void LicenseCheckAbort()
{
	lastLicenseCheckStartTime = 0;
	licenseCheckResult        = 0;
}

// 0 --> check passed, > 0 --> check failed, < 0 --> inconclusive (not possible to check)
int LicenseCheckResultStatus()
{
	return licenseCheckResult;
}

void LicenseCheckResultDetail(char *str, int maxlen)
{
	if (licenseCheckResult == 0)
	{
		strcpy(str, "License not checked yet!");
	}
	else if (licenseCheckResult == 1)
	{
		strcpy(str, "Licensed.");
	}
	else
	{
		strcpy(str, "Unlicensed.");
	}
}

