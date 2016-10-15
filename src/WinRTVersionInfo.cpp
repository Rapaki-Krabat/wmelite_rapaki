
#include "WinRTVersionInfo.h"

#include <cvt/wstring>
#include <codecvt>

#include <string.h>

using namespace Windows::System::Profile;

extern "C" void WinRTVersionInfo(char *info, int maxlen)
{
	size_t currLen = 0;
	stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

	AnalyticsVersionInfo ^ versionInfo = AnalyticsInfo::VersionInfo;

	Platform::String ^ deviceFamily = versionInfo->DeviceFamily;
	Platform::String ^ deviceFamilyVersion = versionInfo->DeviceFamilyVersion;

	std::string deviceFamilyUtf8 = convert.to_bytes(deviceFamily->Data());
	std::string deviceFamilyVersionUtf8 = convert.to_bytes(deviceFamilyVersion->Data());

	const char* deviceFamilyCString = deviceFamilyUtf8.c_str();
	const char* deviceFamilyVersionCString = deviceFamilyVersionUtf8.c_str();

	//	SDL_Log("%s\n", deviceFamilyCString);
	//	SDL_Log("%s\n", deviceFamilyVersionCString);

	info[0] = 0;

	currLen = strlen(deviceFamilyCString);

	if (currLen < (maxlen - 1))
	{
		strcat(info, deviceFamilyCString);
	}

	if (currLen < (maxlen - 1))
	{
		strcat(info, ":");
		currLen++;
	}

	currLen += strlen(deviceFamilyVersionCString);

	if (currLen < (maxlen - 1))
	{
		strcat(info, deviceFamilyVersionCString);
	}

	//	SDL_Log("%s\n", info);
}
