#ifndef WINRT_DETECT_POINTERS_H
#define WINRT_DETECT_POINTERS_H

#include "SDL.h"

extern "C" void LogPointerDevices();

extern "C" int DetectMousePresence();

extern "C" int DetectTouchPresence();

#endif // WINRT_DETECT_POINTERS_H
