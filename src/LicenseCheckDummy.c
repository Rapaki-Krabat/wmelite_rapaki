
#include <string.h>

#include "LicenseCheck.h"

void LicenseCheckStartAsync()
{
       
}

// 0 --> not pending, != 0 --> pending
int LicenseCheckPollStatus()
{
    return 0;   
}

// 0 --> check passed, != 0 --> check failed
int LicenseCheckResultStatus()
{
    return 0;   
}

void LicenseCheckResultDetail(char *str, int maxlen)
{
    strcpy(str, "dummy licnese check status");
}

