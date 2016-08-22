
#include "DisplayConfig.h"

void SetKeepDisplayOn(int keepOn)
{
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
}
