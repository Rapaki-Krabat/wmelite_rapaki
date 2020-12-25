/*
This file is part of WME Lite.
http://dead-code.org/redir.php?target=wmelite

Copyright (c) 2011 Jan Nedoma

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/**
* @file Logger.cpp
*
*/

#include "Logger.h"

#include <ctime>

#include "dctypes.h"
#include "FileOperations.h"
#include "DirectoryOperations.h"
#include "PathUtil.h"

void Logger::switchLogging(bool on, const char* Filename)
{
	if (m_DEBUG_LogFile != NULL)
	{
		LOG(0, "%s", "********** DEBUG LOG CLOSED ********************************************");
		fclose(m_DEBUG_LogFile);
		m_DEBUG_LogFile = NULL;
	}

	if (on == true)
	{
		AnsiString userDir = PathUtil::GetUserDirectory();
		AnsiString logFile = PathUtil::Combine(userDir, Filename);
		SDL_Log("Trying to open log file: %s.\n", logFile.c_str());

		m_DEBUG_LogFile = fopen(logFile.c_str(), "w");
	}
}

void Logger::LOG(HRESULT res, LPCSTR fmt, char* buff)
{
#if !defined(__IPHONEOS__) && !defined(__ANDROID__) && !defined(__WINRT__)
	if (!m_WriteLog) return;
#endif
	time_t timeNow;
	time(&timeNow);
	struct tm* tm = localtime(&timeNow);

	// print to console
	SDL_Log("%02d:%02d:%02d: %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, buff);

//#if defined(__MACOSX__)
//	printf("%02d:%02d:%02d: %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, buff);
//	fflush(stdout);
//#else

//#ifdef __ANDROID__
	// that one should not have performance impact
//	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "%02d:%02d:%02d: %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, buff);
//#endif

//#if defined(__WINRT__)
//	SDL_Log("%02d:%02d:%02d: %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, buff);
//#endif

//#if defined(__IPHONEOS__)
	// one shows log info also when run on "other" devices

//	os_log_info(OS_LOG_DEFAULT, "%02d:%02d:%02d: %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, buff);

	// os_log_info(OS_LOG_DEFAULT, "%02d:%02d:%02d: %{public}s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, buff);
// #endif

	// don't write to any file that is not open
	if (m_DEBUG_LogFile == NULL) return;

	fprintf(m_DEBUG_LogFile, "%02d:%02d: %s\n", tm->tm_hour, tm->tm_min, buff);
	fflush(m_DEBUG_LogFile);

// #endif

	//QuickMessage(buff);
}
