
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
