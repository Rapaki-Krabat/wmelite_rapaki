
#include <string.h>

#include "LicenseCheck.h"

#include "SDL_log.h"

#include <ppltasks.h>

using namespace Platform;
using namespace Windows::ApplicationModel::Store;
using namespace concurrency;

static int lastLicenseCheckStartTime = 0;

static int licenseCheckResult = 0;

// license information
// https://github.com/Microsoft/Windows-universal-samples/blob/win10-1507/Samples/Store/cpp/Scenario1_TrialMode.xaml.cpp

// async task
// https://msdn.microsoft.com/de-de/library/hh750082.aspx

void LicenseCheckStartAsync(int startTime)
{
	lastLicenseCheckStartTime = startTime;

	licenseCheckResult = 0;

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
}

int LicenseCheckStartTime()
{
	return lastLicenseCheckStartTime;
}

// 0 --> not pending, != 0 --> pending
int LicenseCheckPollStatus()
{
	return (licenseCheckResult == 0) ? 1 : 0;
}

void LicenseCheckAbort()
{
	lastLicenseCheckStartTime = 0;
	licenseCheckResult        = 0;
}

// 0 --> check passed, != 0 --> check failed
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

