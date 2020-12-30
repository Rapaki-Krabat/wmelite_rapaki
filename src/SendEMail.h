#ifndef WMELITE_SEND_EMAIL_H
#define WMELITE_SEND_EMAIL_H

#include "dctypes.h"

class SendEmail
{
public:
	static void SendMailWithAttachments(const AnsiString& logfileDir, const AnsiString& savegameDir);
};

#endif // WMELITE_SEND_EMAIL_H
