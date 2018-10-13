
#include <string.h>

#include "LicenseCheck.h"

static int lastLicenseCheckStartTime = 0;

void LicenseCheckStartAsync(int startTime)
{
	lastLicenseCheckStartTime = startTime;
}

int LicenseCheckStartTime()
{
	return lastLicenseCheckStartTime;
}

// 0 --> not pending, != 0 --> pending
int LicenseCheckPollStatus()
{
	return 0;
}

void LicenseCheckAbort()
{
	lastLicenseCheckStartTime = 0;
}

// 0 --> check passed, != 0 --> check failed
int LicenseCheckResultStatus()
{
    return 0;   
}

void LicenseCheckResultDetail(char *str, int maxlen)
{
    int c;
	strcpy(str, "dummy license check status");
    c = maxlen;
}

