
#include "DisplayConfig.h"

#include "SDL.h"
#include "SDL_log.h"

#if 0

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

#else

// use SDLs internal implementation which should be way superior to our own improvised one

void SetKeepDisplayOn(int keepOn)
{
	if (keepOn != 0)
	{
		SDL_DisableScreenSaver();
	}
	else
	{
		SDL_EnableScreenSaver();
	}
}

#endif
