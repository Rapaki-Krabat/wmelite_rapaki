
#include "SendEmail.h"
#include "PathUtil.h"

#include "SDL.h"

#include <ppltasks.h>
#include <collection.h>

using namespace Windows::ApplicationModel::Email;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Foundation;
using namespace Concurrency;
using namespace Platform::Collections;


void SendEmail::SendMailWithAttachments(const AnsiString& logfileDir, const AnsiString& savegameDir)
{
	auto emailMessage = ref new EmailMessage();

	emailMessage->To->Append(ref new EmailRecipient("kontakt@rapaki.de"));
	emailMessage->Subject = "Bug report";
	emailMessage->Body = "Please check the attached data and then press 'send'. Add any comments here.";
	
	Vector<EmailAttachment^>^ attachments = ref new Vector<EmailAttachment^>();

	AnsiStringList logFileList;
	PathUtil::GetFilesInDirectory(logfileDir, AnsiString("*") + AnsiString(".log"), logFileList);

	for (AnsiString file : logFileList)
	{ 
		AnsiString absFile = logfileDir + file;
		std::replace(absFile.begin(), absFile.end(), L'/', L'\\');

		SDL_Log("Trying to attach log file '%s' to email.\n", absFile.c_str());

		std::wstring wstr = std::wstring(absFile.begin(), absFile.end());
		const wchar_t* w_char = wstr.c_str();
		Platform::String^ p_string = ref new Platform::String(w_char);
		auto fileop = StorageFile::GetFileFromPathAsync(p_string);
		
		create_task(fileop).then([attachments](StorageFile^ asyncFile) {
			auto stream = Streams::RandomAccessStreamReference::CreateFromFile(asyncFile);
			EmailAttachment ^ attachment = ref new EmailAttachment(asyncFile->Name, stream);
			attachments->Append(attachment);
			SDL_Log("Attachment added to vector.\n");
		}).then([](task<void> t)
		{
			try {
				t.get();
				// .get() didn't throw, so we succeeded.
				SDL_Log("Attach tread finished.\n");
			}
			catch (Platform::Exception^ exception) {
				// handle error
				SDL_Log("Error thrown from attaching log file to email.\n");
			}

		});
	}

	AnsiStringList saveGameList;
	PathUtil::GetFilesInDirectory(savegameDir, AnsiString("*"), saveGameList);

	for (AnsiString file : saveGameList)
	{
		AnsiString absFile = savegameDir + AnsiString("\\") + file;
		std::replace(absFile.begin(), absFile.end(), L'/', L'\\');
		SDL_Log("Trying to attach savegame file '%s' to email.\n", absFile.c_str());

		std::wstring wstr = std::wstring(absFile.begin(), absFile.end());
		const wchar_t* w_char = wstr.c_str();
		Platform::String^ p_string = ref new Platform::String(w_char);
		auto fileop = StorageFile::GetFileFromPathAsync(p_string);

		create_task(fileop).then([attachments](StorageFile^ asyncFile) {
			auto stream = Streams::RandomAccessStreamReference::CreateFromFile(asyncFile);
			EmailAttachment ^ attachment = ref new EmailAttachment(asyncFile->Name, stream);
			attachments->Append(attachment);
			SDL_Log("Attachment added to vector.\n");
			}).then([](task<void> t)
				{
					try {
						t.get();
						// .get() didn't throw, so we succeeded.
						SDL_Log("Attach tread finished.\n");
					}
					catch (Platform::Exception^ exception) {
						// handle error
						SDL_Log("Error thrown from attaching savegame file to email.\n");
					}

				});
	}

	Sleep(10000);

	while (attachments->Size > 0)
	{
		emailMessage->Attachments->Append(attachments->GetAt(0));
	}

	IAsyncAction ^ result = EmailManager::ShowComposeNewEmailAsync(emailMessage);
}
