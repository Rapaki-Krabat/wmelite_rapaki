
#include "DisplayConfig.h"

#include "SDL_log.h"

using namespace Windows::System::Display;

DisplayRequest ^ request = nullptr;

void SetKeepDisplayOn(int keepOn)
{

	if (request == nullptr)
	{
		request = ref new DisplayRequest();
		if (keepOn != 0)
		{
			request->RequestActive();
		}
		else
		{
			SDL_Log("Error: Trying to release display although not acquired!\n");
		}
	}
	else
	{
		if (keepOn == 0)
		{
			request->RequestRelease();
			request = nullptr;
		}
		else
		{
			SDL_Log("Error: Trying to acquire display although already acquired!\n");
		}
	}
}
