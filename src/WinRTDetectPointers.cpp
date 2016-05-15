
#include "WinRTDetectPointers.h"

extern "C" void LogPointerDevices()
{
	using namespace Windows::Devices::Input;
	auto allDevices = PointerDevice::GetPointerDevices();
	SDL_Log("num devices: %u", allDevices->Size);
	for (unsigned int i = 0; i < allDevices->Size; ++i) {
		PointerDevice ^ dev = allDevices->GetAt(i);
		const char * devTypeName = "unknown";
		switch (dev->PointerDeviceType) {
		case PointerDeviceType::Mouse:   
			devTypeName = "Mouse";
			break;
		case PointerDeviceType::Pen:   
			devTypeName = "Pen"; 
			break;
		case PointerDeviceType::Touch:   
			devTypeName = "Touch"; 
			break;
		}
		SDL_Log("device %u: %s", i, devTypeName);
	}

	MouseCapabilities ^ mCaps = ref new MouseCapabilities();
	SDL_Log("Mouse present: %d.\n", mCaps->MousePresent);

	TouchCapabilities ^ tCaps = ref new TouchCapabilities();
	SDL_Log("Touch present: %d.\n", tCaps->TouchPresent);

}

extern "C" int DetectMousePresence()
{
	using namespace Windows::Devices::Input;

	int mousePresent;

	MouseCapabilities ^ mCaps = ref new MouseCapabilities();

	mousePresent = mCaps->MousePresent;

	return mousePresent;
}

extern "C" int DetectTouchPresence()
{
	using namespace Windows::Devices::Input;

	int touchPresent;

	TouchCapabilities ^ tCaps = ref new TouchCapabilities();

	touchPresent = tCaps->TouchPresent;

	return touchPresent;
}
