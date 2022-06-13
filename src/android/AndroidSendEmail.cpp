
#include "SendEMail.h"
#include "android.h"
#include "dctypes.h"

void SendEmail::SendMailWithAttachments(const AnsiString& logfileDir, const AnsiString& savegameDir)
{
    android_sendSupportEmail(logfileDir.c_str(), savegameDir.c_str());
}
