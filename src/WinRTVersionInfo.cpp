
#include "WinRTVersionInfo.h"

// #include <cvt/wstring>
#include <codecvt>

#include <string.h>

extern "C" void WinRTVersionInfo(char *info, int maxlen)
{
	using namespace Windows::System::Profile;

	size_t currLen = 0;
	// stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

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

extern "C" void WinRTAppInfo(char *info, int maxlen)
{
	using namespace Windows::ApplicationModel;

	size_t currLen = 0;
	// stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;

	Package^ package = Windows::ApplicationModel::Package::Current;
	PackageId^ packageId = package->Id;
	PackageVersion version = packageId->Version;
	
	Platform::String^ output = version.Major.ToString() + "." 
		+ version.Minor.ToString() + "." 
		+ version.Build.ToString() + "." 
		+ version.Revision.ToString();

	std::string appInfoUtf8 = convert.to_bytes(output->Data());

	const char* appInfoCString = appInfoUtf8.c_str();

	info[0] = 0;

	currLen = strlen(appInfoCString);

	if (currLen < (maxlen - 1))
	{
		strcat(info, appInfoCString);
	}
}
