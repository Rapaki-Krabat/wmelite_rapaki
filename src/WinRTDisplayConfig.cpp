
#include "DisplayConfig.h"

void SetKeepDisplayOn(int keepOn)
{
#if 0
	using namespace Windows::System::Display;

	DisplayRequest ^ request = ref new DisplayRequest();

	if (keepOn != 0)
	{
		request->RequestActive();
	}
	else
	{
		request->RequestRelease();
	}
#endif
}
