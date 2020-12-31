
#include "SendEMail.h"

#include "SendEmailIOSITF.h"

void SendEmail::SendMailWithAttachments(const AnsiString& logfileDir, const AnsiString& savegameDir)
{
    IOS_SendMailWithAttachments(NULL, 0, NULL, 0);
}
