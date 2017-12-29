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

#include "dcgf.h"
#include "BRenderSDL.h"
#include "BSurfaceSDL.h"
#include "FreeImage.h"
#include "MathUtil.h"
#include "StringUtil.h"
#include <math.h>
#include "SDL_syswm.h"

#include "ios_utils.h"

#ifdef __IPHONEOS__
// #define EMULATE_VIEWPORT_SETTING
#endif

//////////////////////////////////////////////////////////////////////////
CBRenderSDL::CBRenderSDL(CBGame* inGame) : CBRenderer(inGame)
{
	m_Texture = NULL;
	m_Renderer = NULL;
	m_Win = NULL;

	m_PhysicalBorderLeft = m_PhysicalBorderRight = m_PhysicalBorderTop = m_PhysicalBorderBottom = 0;
	m_LogicalRatioX = m_LogicalRatioY = 1.0f;
	m_PhysicalRatioX = m_PhysicalRatioY = 1.0f;
	m_PhysicalWidth = m_PhysicalHeight = m_LogicalWidth = m_LogicalHeight = 0;
    m_LogicalBorderOffsetLeft = m_LogicalBorderOffsetRight = m_LogicalBorderOffsetTop = m_LogicalBorderOffsetBottom = 0;
	m_PixelPerfect = false;

	m_ProgressIndicator = -1;
}

//////////////////////////////////////////////////////////////////////////
CBRenderSDL::~CBRenderSDL()
{
	if (m_Texture) SDL_DestroyTexture(m_Texture);
	if (m_Renderer) SDL_DestroyRenderer(m_Renderer);
	if (m_Win) SDL_DestroyWindow(m_Win);
	
	SDL_Quit();
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::InitRenderer(int width, int height, bool windowed, float upScalingRatioStepping, float downScalingRatioStepping, bool pixelPerfectRendering, const AnsiString& renderingHint, bool vsync, bool m_DebugAlwaysShowPlatformCursor)
{
	bool desktopModeRequested;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Game->LOG(0, "SDL init video failed???\n");
		return E_FAIL;
	}
	
	m_upScalingRatioStepping   = upScalingRatioStepping;
	m_downScalingRatioStepping = downScalingRatioStepping;

	const SDL_DisplayMode* current = NULL;

	// for searching matching resolution, always remember "best" pick
	SDL_DisplayMode bestResolution;
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);

	bool doubleBuffering = Game->m_Registry->ReadBool("Rendering", "DoubleBuffering", false);

	Game->LOG(0, "Double buffering flag set to: %s.", (doubleBuffering) ? "ON" : "default");
	if (doubleBuffering)
	{
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	}

	int deviceNumber = Game->m_Registry->ReadInt("Video", "DeviceNumber", 0);

	Game->LOG(0, "Current video device no=%d, max=%d (- 1).", deviceNumber, SDL_GetNumVideoDisplays());

	m_ProgressIndicator         = Game->m_Registry->ReadInt("Video", "ProgressIndicatorFrames", -1);
	m_MaxProgressIndicator      = m_ProgressIndicator;
	m_ProgressIndicatorRed      = (BYTE) Game->m_Registry->ReadInt("Video", "ProgressIndicatorRed", 0);
	m_ProgressIndicatorGreen    = (BYTE) Game->m_Registry->ReadInt("Video", "ProgressIndicatorGreen", 0x80);
	m_ProgressIndicatorBlue     = (BYTE) Game->m_Registry->ReadInt("Video", "ProgressIndicatorBlue", 0);
	m_ProgressIndicatorNumLines = Game->m_Registry->ReadInt("Video", "ProgressIndicatorNumLines", 3);

	m_Width = width;
	m_Height = height;

	m_LogicalWidth  = 0;
	m_LogicalHeight = 0;

	m_NewLogicalWidth = width;
	m_NewLogicalHeight = height;

	m_PixelPerfect = pixelPerfectRendering;

	desktopModeRequested = Game->m_Registry->ReadBool("Video", "DesktopResolution", false);

#if defined(__IPHONEOS__) || defined(__ANDROID__) || defined(__WINRT__)
	// always desire fullscreen at desktop resolution for mobile platforms
	desktopModeRequested = true;
#endif

	// find suitable resolution
#if defined(__IPHONEOS__) || defined(__ANDROID__)
	// mobile devices: search all resolutions until one is found where width > height

	m_NewLogicalWidth = 480;
	m_NewLogicalHeight = 320;

	int numModes = SDL_GetNumDisplayModes(deviceNumber);

    Game->LOG(0, "iOS / Android report %d modes.", numModes);
    
    for (int i = 0; i < numModes; i++)
	{
		SDL_GetDisplayMode(deviceNumber, i, &bestResolution);

        Game->LOG(0, "iOS / Android mode %d x=%d y=%d.", i, bestResolution.w, bestResolution.h);

		// use new mode if it matches, but make sure that at least one mode is picked in the end
		if (((bestResolution.w > bestResolution.h) && ((bestResolution.w > m_NewLogicalWidth) || (bestResolution.h > m_NewLogicalHeight))) || (current == NULL))
		{
			m_NewLogicalWidth     = bestResolution.w;
			m_NewLogicalHeight    = bestResolution.h;
			current = &bestResolution;
		}
	}	
#else
	// windows/linux: search a resolution with same screen width/height ratio
	// inspired by Jan Kavan (wmelite julia branch)
	SDL_DisplayMode testResolution;

	SDL_GetCurrentDisplayMode(deviceNumber, &bestResolution);

	// compute ratios precisely
	float gameRatio = (float)width / (float)height;
	float bestRatio = (float)bestResolution.w / (float)bestResolution.h;
	float bestRatioDiff = gameRatio - bestRatio;

	Game->LOG(0, "Game dimension=%dx%d, ratio=%.2f.", width, height, gameRatio);

	Game->LOG(0, "Starting best screen dimension=%dx%d, ratio=%.2f, ratio diff=%.2f.", bestResolution.w, bestResolution.h, bestRatio, bestRatioDiff);

	if (desktopModeRequested == false)
	{
		// now iterate over all modes of this screen and pick the best one
		for (int i = 0; i < SDL_GetNumDisplayModes(deviceNumber); i++)
		{
			if (SDL_GetDisplayMode(deviceNumber, i, &testResolution) == 0)
			{
				 float testRatio = (float)testResolution.w / (float)testResolution.h;
				 float testRatioDiff = gameRatio - testRatio;

				 Game->LOG(0, "Other screen dimension=%dx%d, ratio=%.2f, ratio diff=%.2f.", testResolution.w, testResolution.h, testRatio, testRatioDiff);

				 // the current screen ratio is not equal to the last best one
				 if (MathUtil::FloatsAreEqual(bestRatioDiff, testRatioDiff) == false)
				 {
					// new mode is bigger than game resolution (do not pick different mode that needs downscaling)
					if ((testResolution.w >= width) && (testResolution.h >= height))
					{
						// the ratio fits better
						if (fabs(testRatioDiff) < fabs(bestRatioDiff))
						{
							Game->LOG(0, "New mode better than current (%.2f < %.2f), picking this one!", testRatioDiff, bestRatioDiff);

							bestRatioDiff  = testRatioDiff;
							bestRatio      = testRatio;
							bestResolution = testResolution;
						}
					}
				 }
			}
		}
	}
	else
	{
		Game->LOG(0, "Desktop mode forced!");
	}

	// this is our resulting mode
	current = &bestResolution;

	m_NewLogicalWidth = current->w;
	m_NewLogicalHeight = current->h;

#ifdef __WINRT__
	// always assume ccords in landscape for WinRT
	if (m_NewLogicalWidth < m_NewLogicalHeight)
	{
		m_NewLogicalWidth  = current->h;
		m_NewLogicalHeight = current->w;
	}
#endif

#endif

	// last chance to override resolution settings with registry
	if ((Game->m_Registry->ReadInt("Debug", "ForceResWidth", 0) != 0) && (Game->m_Registry->ReadInt("Debug", "ForceResHeight", 0) != 0)) {
		m_NewLogicalWidth  = Game->m_Registry->ReadInt("Debug", "ForceResWidth", m_Width);
		m_NewLogicalHeight = Game->m_Registry->ReadInt("Debug", "ForceResHeight", m_Height);
	}

	// density is in "points", and "diag" is the scale factor logical/physical (iOS)
	float fDensityDiag;
	float fDensityX;
	float fDensityY;
#if !defined(__WIN32__) || defined(__MINGW32__)
	if (SDL_GetDisplayDPI(deviceNumber, &fDensityDiag, &fDensityX, &fDensityY) == 0)
	{
		m_PhysicalDensityX = (int) fDensityX;
		m_PhysicalDensityY = (int) fDensityY;
	}
	else
#endif
	{
		fDensityDiag = 1;
		m_PhysicalDensityX = 0;
		m_PhysicalDensityY = 0;
	}

#ifdef __IPHONEOS__
	// if a scale factor is reported (iOS Retina displays), update the densities appropriately

    Game->LOG(0, "Updating densities by scale factor: %.2f.", fDensityDiag);

    // density values change as per reported scale
    m_PhysicalDensityX *= fDensityDiag;
    m_PhysicalDensityY *= fDensityDiag;

#endif

	Uint32 flags = SDL_WINDOW_SHOWN;
#ifdef __IPHONEOS__
	flags |= SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALLOW_HIGHDPI;
#else
#ifdef __ANDROID__
	// just do the same IOS does
	flags |= SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS;
#endif

#endif
	// registry wins over command line
	m_Windowed = Game->m_Registry->ReadBool("Video", "Windowed", windowed);
    
	// as of Novmber 2012 using SDL_WINDOW_FULLSCREEN causes iOS glitch when returning back to the app (the image is shifted several pixels up)
	// so better don't use it on iOS
	if (!m_Windowed) flags |= SDL_WINDOW_FULLSCREEN;

	// mobile platforms would want the native/desktop/onlyonesupported resolution
	if (desktopModeRequested == true) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	m_Win = SDL_CreateWindow("WME Lite",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_NewLogicalWidth, m_NewLogicalHeight,
		flags);
	
	if (!m_Win) {
		Game->LOG(0, "Fatal: Could not create SDL window???\n");
		return E_FAIL;
	}

	SDL_SetWindowDisplayMode(m_Win, current);	

	if (m_DebugAlwaysShowPlatformCursor == false)
	{
		SDL_ShowCursor(SDL_DISABLE);
	}

#ifdef __IPHONEOS__
	// SDL defaults to OGL ES2, which doesn't work on old devices
    // 20151219: changed to opengles2, assume all active devices will work
	AnsiString defaultRenderingHint = "opengles2";
    
#else

#ifdef __ANDROID__
	// lets assume almost every active device has OpenGL ES2 now...
	AnsiString defaultRenderingHint = "opengles2";
#else

#if defined(__WIN32__) || defined(__WINRT__)
    // directx for Windows
	AnsiString defaultRenderingHint = "direct3d";

#else
	// OpenGL for Linux and all others
	AnsiString defaultRenderingHint = "opengl";

#endif

#endif

#endif

	if (StringUtil::CompareNoCase(renderingHint, "default")) {
		Game->LOG(0, "Set rendering hint default value: %s", defaultRenderingHint.c_str());
		SDL_SetHint(SDL_HINT_RENDER_DRIVER, defaultRenderingHint.c_str());
	} else {
		Game->LOG(0, "Set rendering hint registry value: %s", renderingHint.c_str());
		SDL_SetHint(SDL_HINT_RENDER_DRIVER, renderingHint.c_str());
	}


	if (vsync) SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

#ifdef __ANDROID__

	SDL_SetHint(SDL_HINT_ANDROID_HIDE_SYSTEM_BARS, "1");

#endif

	// m_Renderer = SDL_CreateRenderer(m_Win, -1, SDL_RENDERER_PRESENTVSYNC);
	m_Renderer = SDL_CreateRenderer(m_Win, -1, 0);
	if (!m_Renderer) return E_FAIL;

#ifdef __IPHONEOS__
    
    // with iphone x and ios 11 there is a bar rendered on top of all content
    // to avoid important info to be hidden beneath, query the "safe layout" area
    // and restrict rendering to this - rest will stay black
    
    SDL_SysWMinfo wmInfo;
    
    SDL_VERSION(&wmInfo.version); /* initialize info structure with SDL version info */
    
    if(SDL_GetWindowWMInfo(m_Win, &wmInfo)) { /* the call returns true on success */
        // info.info.uikit.window;
        // Game->LOG(0, "get wm info success!\n");
        
        IOS_GetSafeLayoutLogicalOffsets(&wmInfo, m_NewLogicalWidth, m_NewLogicalHeight, &m_LogicalBorderOffsetLeft, &m_LogicalBorderOffsetTop, &m_LogicalBorderOffsetRight, &m_LogicalBorderOffsetBottom);
    }
    
#endif
    
	SDL_RendererInfo info;
	SDL_GetRendererInfo(m_Renderer, &info);
	Game->LOG(0, "Renderer max texture sizes: x=%d y=%d.\n", info.max_texture_width, info.max_texture_height);

	// we have a renderer, now we can compute all the scaling values
	UpdateWindowScaling();

	if (m_PixelPerfect) {
		// try to make texture scaling look as nice as possible
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
		m_Texture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_Width, m_Height);
		if (!m_Texture) return E_FAIL;
		SDL_SetTextureBlendMode(m_Texture, SDL_BLENDMODE_BLEND);
		// SDL_SetRenderTarget(m_Renderer, m_Texture);
		// SDL_RenderSetViewport(m_Renderer, NULL);

		Game->AddMem(m_Width * m_Height * 4);

		m_RenderOffscreen = true;

		Game->LOG(0, "PixelPerfect rendering enabled!");
	} else {
		m_RenderOffscreen = false;

		SDL_RenderSetViewport(m_Renderer, NULL);

		Game->LOG(0, "PixelPerfect rendering disabled!");
	}

	// necessary init and computations for experimental frame rate limit
	m_LastRenderTime = 0;
	m_SecondTickTime = 0;
	m_FrameCounter   = 0;
	m_FrameRateLimit = Game->m_Registry->ReadInt("Rendering", "FrameRateLimit", 0);
	
	// be as little invasive as possible, if the framerate is already ok or even too low
	// there's no need to sleep
	m_FrameRateLimitBrakeActive = false;
	m_AccumulatedSleepTime = 0;
	
	if (m_FrameRateLimit > 0)
	{
		m_FrameSleepTime = MathUtil::Round(1000.0f / ((float) m_FrameRateLimit));
	}

	m_Active = true;

	return S_OK;
}

/////////////////////////////////////////////////////
void CBRenderSDL::WindowResized(int x, int y)
{
#ifdef __WINRT__
	// work-around an SDL bug where the window size is sometimes reported in portrait and sometimes in landcape mode
	if (m_LogicalWidth > m_LogicalHeight)
	{
		// our initial orientation was landscape
		if (x > y)
		{
			m_NewLogicalWidth  = x;
			m_NewLogicalHeight = y;
		}
		else
		{
			m_NewLogicalWidth  = y;
			m_NewLogicalHeight = x;
		}
	}
	else
	{
		// our initial orientation was portrait
		if (x < y)
		{
			m_NewLogicalWidth  = x;
			m_NewLogicalHeight = y;
		}
		else
		{
			m_NewLogicalWidth  = y;
			m_NewLogicalHeight = x;
		}
	}
#else
	// re-computation will be done next time the window is erased
	m_NewLogicalWidth  = x;
	m_NewLogicalHeight = y;
#endif
}

/////////////////////////////////////////////////////
void CBRenderSDL::UpdateWindowScaling()
{
#ifdef __IPHONEOS__
    // if a high DPI window was created, we need to query sites in pixels from the current renderer
	// make sure that this function is not called when the renderer is set for offscreen rendering
    int rendererX;
    int rendererY;

    if (SDL_GetRendererOutputSize(m_Renderer, &rendererX, &rendererY) == 0)
    {
    	m_PhysicalWidth  = rendererX;
    	m_PhysicalHeight = rendererY;

        Game->LOG(0, "Query renderer: x=%d y=%d.", rendererX, rendererY);
    }
    else
    {
    	m_PhysicalWidth  = m_NewLogicalWidth;
    	m_PhysicalHeight = m_NewLogicalHeight;

        Game->LOG(0, "Renderer query failed, using logical sizes x=%d y=%d.", m_NewLogicalWidth, m_NewLogicalHeight);
    }
#else
	m_PhysicalWidth  = m_NewLogicalWidth;
	m_PhysicalHeight = m_NewLogicalHeight;
#endif

	m_LogicalWidth  = m_NewLogicalWidth;
	m_LogicalHeight = m_NewLogicalHeight;

    float origAspect         = (float) m_Width         / (float) m_Height;
	float realLogicalAspect  = (float) m_LogicalWidth  / (float) m_LogicalHeight;
	// float realPhysicalAspect = (float) m_PhysicalWidth / (float) m_PhysicalHeight;

	float logicalRatio;
	float physicalRatio;
	if (origAspect < realLogicalAspect)
	{
		// normal to wide
		logicalRatio  = (float) m_LogicalHeight  / (float) m_Height;
		physicalRatio = (float) m_PhysicalHeight / (float) m_Height;
	}
	else
	{
		// wide to normal
		logicalRatio  = (float) m_LogicalWidth  / (float) m_Width;
		physicalRatio = (float) m_PhysicalWidth / (float) m_Width;
	}

	Game->LOG(0, "Ratios before stepping application: %.02f %.02f", logicalRatio, physicalRatio);

	if (logicalRatio > 1.0) {
		logicalRatio = GetAlignedUpscalingRatio(logicalRatio, m_upScalingRatioStepping);
	}
	if (logicalRatio < 1.0) {
		logicalRatio = GetAlignedDownscalingRatio(logicalRatio, m_downScalingRatioStepping);
	}
	if (physicalRatio > 1.0) {
		physicalRatio = GetAlignedUpscalingRatio(physicalRatio, m_upScalingRatioStepping);
	}
	if (physicalRatio < 1.0) {
		physicalRatio = GetAlignedDownscalingRatio(physicalRatio, m_downScalingRatioStepping);
	}

	Game->LOG(0, "Ratios after stepping application: %.02f %.02f", logicalRatio, physicalRatio);

	// logical coordinates update
	m_LogicalBorderLeft  = (m_LogicalWidth - (m_Width * logicalRatio)) / 2;
	m_LogicalBorderRight =  m_LogicalWidth - (m_Width * logicalRatio) - m_LogicalBorderLeft;
    
    /*
    printf("Computed logical left border = %d and safe area offset = %d.\n", m_LogicalBorderLeft, m_LogicalBorderOffsetLeft);
    printf("Computed logical right border = %d and safe area offset = %d.\n", m_LogicalBorderRight, m_LogicalBorderOffsetRight);
    */
    
    if (m_LogicalBorderLeft < m_LogicalBorderOffsetLeft) m_LogicalBorderLeft = m_LogicalBorderOffsetLeft;
    if (m_LogicalBorderRight < m_LogicalBorderOffsetRight) m_LogicalBorderRight = m_LogicalBorderOffsetRight;

	m_LogicalBorderTop    = (m_LogicalHeight - (m_Height * logicalRatio)) / 2;
	m_LogicalBorderBottom =  m_LogicalHeight - (m_Height * logicalRatio) - m_LogicalBorderTop;

    /*
    printf("Computed logical top border = %d and safe area offset = %d.\n", m_LogicalBorderTop, m_LogicalBorderOffsetTop);
    printf("Computed logical bottom border = %d and safe area offset = %d.\n", m_LogicalBorderBottom, m_LogicalBorderOffsetBottom);
    */
    
    if (m_LogicalBorderTop < m_LogicalBorderOffsetTop) m_LogicalBorderTop = m_LogicalBorderOffsetTop;
    if (m_LogicalBorderBottom < m_LogicalBorderOffsetBottom) m_LogicalBorderBottom = m_LogicalBorderOffsetBottom;

	m_LogicalRatioX = (float)(m_LogicalWidth  - m_LogicalBorderLeft - m_LogicalBorderRight)  / (float) m_Width;
	m_LogicalRatioY = (float)(m_LogicalHeight - m_LogicalBorderTop  - m_LogicalBorderBottom) / (float) m_Height;

	Game->LOG(0, "Update screen: Orig w=%d h=%d Logical w=%d h=%d ratiox=%.02f ratioy=%.02f",
			m_Width, m_Height, m_LogicalWidth, m_LogicalHeight,
			m_LogicalRatioX, m_LogicalRatioY);
	Game->LOG(0, "Update screen: Logical BorderLeft=%d BorderRight=%d BorderTop=%d BorderBottom=%d",
			m_LogicalBorderLeft, m_LogicalBorderRight, m_LogicalBorderTop, m_LogicalBorderBottom);

	// physical coordinates update
	m_PhysicalBorderLeft  = (m_PhysicalWidth - (m_Width * physicalRatio)) / 2;
	m_PhysicalBorderRight =  m_PhysicalWidth - (m_Width * physicalRatio) - m_PhysicalBorderLeft;

	m_PhysicalBorderTop    = (m_PhysicalHeight - (m_Height * physicalRatio)) / 2;
	m_PhysicalBorderBottom =  m_PhysicalHeight - (m_Height * physicalRatio) - m_PhysicalBorderTop;

	m_PhysicalRatioX = (float)(m_PhysicalWidth  - m_PhysicalBorderLeft - m_PhysicalBorderRight)  / (float) m_Width;
	m_PhysicalRatioY = (float)(m_PhysicalHeight - m_PhysicalBorderTop  - m_PhysicalBorderBottom) / (float) m_Height;

	Game->LOG(0, "Update screen: Orig w=%d h=%d Physical w=%d h=%d ratiox=%.02f ratioy=%.02f",
			m_Width, m_Height, m_PhysicalWidth, m_PhysicalHeight,
			m_PhysicalRatioX, m_PhysicalRatioY);
	Game->LOG(0, "Update screen: Physical BorderLeft=%d BorderRight=%d BorderTop=%d BorderBottom=%d",
			m_PhysicalBorderLeft, m_PhysicalBorderRight, m_PhysicalBorderTop, m_PhysicalBorderBottom);

	// in case of pixelperfect rendering, the viewport coords need to be updated
	m_PixelPerfectTargetRect.x = m_PhysicalBorderLeft;
	m_PixelPerfectTargetRect.y = m_PhysicalBorderTop;
	m_PixelPerfectTargetRect.w = m_PhysicalWidth;
	m_PixelPerfectTargetRect.h = m_PhysicalHeight;
}

/////////////////////////////////////////////////////
HRESULT CBRenderSDL::SendRenderingHintSceneComplete()
{
	if (m_PixelPerfect) {
		if (m_RenderOffscreen) {
			SDL_SetRenderTarget(m_Renderer, NULL);
			SDL_RenderSetViewport(m_Renderer, NULL);
			SDL_RenderCopy(m_Renderer, m_Texture, NULL, &m_PixelPerfectTargetRect);

#ifndef EMULATE_VIEWPORT_SETTING
			SDL_RenderSetViewport(m_Renderer, &m_PixelPerfectTargetRect);
#endif

			m_RenderOffscreen = false;
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::Flip()
{
	
#ifdef EMULATE_VIEWPORT_SETTING
	// hack: until viewports work correctly, we just paint black bars instead
    
	SDL_SetRenderDrawColor(m_Renderer, 0x00, 0x00, 0x00, 0xFF);

	static bool firstRefresh = true; // prevents a weird color glitch
	if (firstRefresh)
	{
		firstRefresh = false;
	}
	else
	{
		SDL_Rect rect;
		if (m_BorderLeft > 0)
		{
			rect.x = 0;
			rect.y = 0;
			rect.w = m_BorderLeft;
			rect.h = m_RealHeight;		
			SDL_RenderFillRect(m_Renderer, &rect);
		}
		if (m_BorderRight > 0)
		{
			rect.x = (m_RealWidth - m_BorderRight);
			rect.y = 0;
			rect.w = m_BorderRight;
			rect.h = m_RealHeight;		
			SDL_RenderFillRect(m_Renderer, &rect);
		}
		if (m_BorderTop > 0)
		{
			rect.x = 0;
			rect.y = 0;
			rect.w = m_RealWidth;
			rect.h = m_BorderTop;		
			SDL_RenderFillRect(m_Renderer, &rect);
		}
		if (m_BorderBottom > 0)
		{
			rect.x = 0;
			rect.y = m_RealHeight - m_BorderBottom;
			rect.w = m_RealWidth;
			rect.h = m_BorderBottom;		
			SDL_RenderFillRect(m_Renderer, &rect);
		}
	}
    
#endif

	// if not already done, draw the offscreen image onto the final screen
	SendRenderingHintSceneComplete();

	// last resort to limit frame rate if vsync does not work
	if (m_FrameRateLimit > 0)
	{
		// check how many milliseconds the rendering is "too fast"
		DWORD currRenderTime = CBPlatform::GetTime();
		DWORD renderDiffTime = currRenderTime - m_LastRenderTime;

		// accumulate the "requested" overall sleep time per second
		m_AccumulatedSleepTime += m_FrameSleepTime - renderDiffTime;

		// if the brake is active, sleep here
		if ((m_FrameRateLimitBrakeActive) && (renderDiffTime < m_FrameSleepTime))
		{
			CBPlatform::SleepMs(m_FrameSleepTime - renderDiffTime);
		}
		m_LastRenderTime = CBPlatform::GetTime();

		// once per second, check whether the brake was necessary 
		// and whether the sleep time needs adjustment
		m_FrameCounter++;
		if ((m_LastRenderTime - m_SecondTickTime) > 1000)
		{
			// modify the sleep time if the actual frame rate diverts from the requested one
			if ((m_FrameRateLimitBrakeActive) && (m_FrameCounter != m_FrameRateLimit))
			{
				if (m_FrameCounter > (m_FrameRateLimit + 1))
				{
					m_FrameSleepTime++;
				}
				else 
				{
					if (m_FrameCounter < (m_FrameRateLimit - 1))
					{
						if (m_FrameSleepTime > 0)
						{
							m_FrameSleepTime--;
						}
					}
				}
			}

			// check whether the "brake" needs to be switched on or off
			// depending on the rendering/sleeping time during the last second
			if (m_AccumulatedSleepTime > 60)
			{
				m_FrameRateLimitBrakeActive = true;
			}
			else
			{
				m_FrameRateLimitBrakeActive = false;
			}
			m_AccumulatedSleepTime = 0;

			m_FrameCounter = 0;
			m_SecondTickTime = m_LastRenderTime;
		}
	}


	if (m_ProgressIndicator >= 0)
	{
		// mid-screen
//			int x1 = 0;
//			int y1 = (m_Height / 2) - 5;
//			int x2 = m_Width * (((float) m_ProgressIndicator) / ((float) m_MaxProgressIndicator));
//			int y2 = (m_Height / 2) + 5;

		int x1 = 0;
		int y1 = m_PhysicalHeight - m_PhysicalBorderTop - m_PhysicalBorderBottom - m_ProgressIndicatorNumLines;
		int x2 = (m_PhysicalWidth - m_PhysicalBorderLeft - m_PhysicalBorderRight) * (((float) (m_MaxProgressIndicator - m_ProgressIndicator)) / ((float) m_MaxProgressIndicator));
		int y2 = m_PhysicalHeight - m_PhysicalBorderTop - m_PhysicalBorderBottom - 1;

		// Game->LOG(0, "?Drawing from %d,%d to %d,%d", x1, y1, x2, y2);

		Uint8 alpha = 0xFF * (((float) (m_ProgressIndicator + m_MaxProgressIndicator)) / ((float) (m_MaxProgressIndicator * 2)));

		SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);

		for (int u = y1; u <= y2; u++)
		{
			SDL_SetRenderDrawColor(m_Renderer, m_ProgressIndicatorRed, m_ProgressIndicatorGreen, m_ProgressIndicatorBlue, alpha);

			SDL_RenderDrawLine(m_Renderer, x1, u, x2, u);
		}

		m_ProgressIndicator--;
	}

	SDL_RenderPresent(m_Renderer);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::EraseBackground()
{
	// update values from window update event here to be consistent
	if ((m_NewLogicalWidth != m_LogicalWidth) || (m_NewLogicalHeight != m_LogicalHeight))
	{
		UpdateWindowScaling();
	}

	Fill(0, 0, 0, NULL);

	// in case of pixelperfect rendering, both on- and offscreen
	// image need to be erased
	if (m_PixelPerfect) {
		SDL_SetRenderTarget(m_Renderer, m_Texture);
		SDL_RenderSetViewport(m_Renderer, NULL);
		Fill(0, 0, 0, NULL);
		m_RenderOffscreen = true;
	} else {
		// paranoia
		m_RenderOffscreen = false;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::Fill(BYTE r, BYTE g, BYTE b, RECT* rect)
{
	SDL_SetRenderDrawColor(m_Renderer, r, g, b, 0xFF);
	SDL_RenderClear(m_Renderer);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::Fade(WORD Alpha)
{
	DWORD dwAlpha = 255 - Alpha;
	return FadeToColor(dwAlpha<<24);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::FadeToColor(DWORD Color, RECT* rect)
{
	SDL_Rect fillRect;

	if (rect)
	{
		fillRect.x = rect->left;
		fillRect.y = rect->top;
		fillRect.w = rect->right - rect->left;
		fillRect.h = rect->bottom - rect->top;		
	}
	else
	{
		RECT rc;
		Game->GetCurrentViewportRect(&rc);
		fillRect.x = rc.left;
		fillRect.y = rc.top;
		fillRect.w = rc.right - rc.left;
		fillRect.h = rc.bottom - rc.top;
	}
	ModTargetRect(&fillRect);

	BYTE r = D3DCOLGetR(Color);
	BYTE g = D3DCOLGetG(Color);
	BYTE b = D3DCOLGetB(Color);
	BYTE a = D3DCOLGetA(Color);

	SDL_SetRenderDrawColor(m_Renderer, r, g, b, a);
	SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(m_Renderer, &fillRect);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::DrawLine(int X1, int Y1, int X2, int Y2, DWORD Color)
{
	BYTE r = D3DCOLGetR(Color);
	BYTE g = D3DCOLGetG(Color);
	BYTE b = D3DCOLGetB(Color);
	BYTE a = D3DCOLGetA(Color);

	SDL_SetRenderDrawColor(m_Renderer, r, g, b, a);
	SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);

#ifdef EMULATE_VIEWPORT_SETTING
	// maybe on iOS viewport setting still doesn't work...
	POINT point1, point2;
	point1.x = X1;
	point1.y = Y1;
	PointToScreen(&point1);

	point2.x = X2;
	point2.y = Y2;
	PointToScreen(&point2);
#else
	// setviewport() is respected, so only scale the coordinates
	POINT point1, point2;
	point1.x = X1 * m_PhysicalRatioX;
	point1.y = Y1 * m_PhysicalRatioY;

	point2.x = X2 * m_PhysicalRatioX;
	point2.y = Y2 * m_PhysicalRatioY;
#endif

	SDL_RenderDrawLine(m_Renderer, point1.x, point1.y, point2.x, point2.y);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CBImage* CBRenderSDL::TakeScreenshot()
{
	SDL_Rect viewport;

	SDL_RenderGetViewport(m_Renderer, &viewport);
	
	SDL_Surface* surface = SDL_CreateRGBSurface(0, viewport.w, viewport.h, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, 0x00000000);
	if (!surface) return NULL;

	if (SDL_RenderReadPixels(m_Renderer, NULL, surface->format->format, surface->pixels, surface->pitch) < 0) return NULL;


	FIBITMAP* dib = FreeImage_Allocate(viewport.w, viewport.h, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	
	int bytespp = FreeImage_GetLine(dib) / FreeImage_GetWidth(dib);
	
	for (unsigned y = 0; y < FreeImage_GetHeight(dib); y++)
	{
		BYTE* bits = FreeImage_GetScanLine(dib, y);
		BYTE* src = (BYTE*)surface->pixels + (viewport.h - y - 1) * surface->pitch;
		memcpy(bits, src, bytespp * viewport.w);
	}
	
	return new CBImage(Game, dib);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::ReassureFullscreen()
{
	if (m_Windowed) SDL_SetWindowFullscreen(m_Win, SDL_FALSE);
	else SDL_SetWindowFullscreen(m_Win, SDL_TRUE);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::SwitchFullscreen()
{
	if (m_Windowed) SDL_SetWindowFullscreen(m_Win, SDL_TRUE);
	else SDL_SetWindowFullscreen(m_Win, SDL_FALSE);

	m_Windowed = !m_Windowed;

	Game->m_Registry->WriteBool("Video", "Windowed", m_Windowed);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
const char* CBRenderSDL::GetName()
{
	if (m_Name.empty())
	{
		if (m_Renderer)
		{
			SDL_RendererInfo info;
			SDL_GetRendererInfo(m_Renderer, &info);
			m_Name = AnsiString(info.name);
		}
	}
	return m_Name.c_str();
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderSDL::SetViewport(int left, int top, int right, int bottom)
{
	SDL_Rect rect;
	
	if (m_RenderOffscreen == false)
	{
		// original behaviour --> modify viewport rect
		rect.x = left + m_PhysicalBorderLeft;
		rect.y = top + m_PhysicalBorderTop;
		rect.w = (right - left) * m_PhysicalRatioX;
		rect.h = (bottom - top) * m_PhysicalRatioY;

		// TODO fix this once viewports work correctly in SDL/landscape
#ifndef EMULATE_VIEWPORT_SETTING
		SDL_RenderSetViewport(GetSdlRenderer(), &rect);
#endif
	}
	else
	{
		// pixelperfect behaviour --> remember viewport for final onscreen drawing only
		m_PixelPerfectTargetRect.x = left + m_PhysicalBorderLeft;
		m_PixelPerfectTargetRect.y = top + m_PhysicalBorderTop;
		m_PixelPerfectTargetRect.w = (right - left) * m_PhysicalRatioX;
		m_PixelPerfectTargetRect.h = (bottom - top) * m_PhysicalRatioY;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CBRenderSDL::ModTargetRect(SDL_Rect* rect)
{
	SDL_Rect viewportRect;

	if (m_RenderOffscreen == false)
	{
		// only apply offsets/scaling if pixelperfect rendering is off
		SDL_RenderGetViewport(GetSdlRenderer(), &viewportRect);

		rect->x = MathUtil::Round(rect->x * m_PhysicalRatioX + m_PhysicalBorderLeft - viewportRect.x);
		rect->y = MathUtil::Round(rect->y * m_PhysicalRatioY + m_PhysicalBorderTop - viewportRect.y);
		rect->w = MathUtil::RoundUp(rect->w * m_PhysicalRatioX);
		rect->h = MathUtil::RoundUp(rect->h * m_PhysicalRatioY);
	}
}

//////////////////////////////////////////////////////////////////////////
void CBRenderSDL::ModOrigin(SDL_Point* origin)
{
	// FIXME I don't know if this is correct...
	if (m_RenderOffscreen == false)
	{
		// only apply scaling if no pixelperfect rendering is requested
		origin->x *= m_PhysicalRatioX;
		origin->y *= m_PhysicalRatioY;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBRenderSDL::PointFromScreen(POINT* point)
{
	point->x = (point->x  - m_LogicalBorderLeft) / m_LogicalRatioX;
	point->y = (point->y  - m_LogicalBorderTop) / m_LogicalRatioY;
}

//////////////////////////////////////////////////////////////////////////
void CBRenderSDL::PointToScreen(POINT* point)
{
	point->x = MathUtil::Round((point->x * m_LogicalRatioX) + m_LogicalBorderLeft);
	point->y = MathUtil::Round((point->y * m_LogicalRatioY) + m_LogicalBorderTop);
}

//////////////////////////////////////////////////////////////////////////
void CBRenderSDL::DumpData(char* Filename)
{
	FILE* f = fopen(Filename, "wt");
	if(!f) return;

	CBSurfaceStorage* Mgr = Game->m_SurfaceStorage;

	int TotalKB = 0;
	fprintf(f, "Filename;Usage;Size;KBytes\n");
	for(int i=0; i<Mgr->m_Surfaces.GetSize(); i++)
	{
		CBSurfaceSDL* Surf = (CBSurfaceSDL*)Mgr->m_Surfaces[i];
		if(!Surf->m_Filename) continue;
		if(!Surf->m_Valid) continue;

		fprintf(f, "%s;%d;", Surf->m_Filename, Surf->m_ReferenceCount);
		fprintf(f, "%dx%d;", Surf->GetWidth(), Surf->GetHeight());

		int kb = Surf->GetWidth() * Surf->GetHeight() * 4 / 1024;

		TotalKB+=kb;
		fprintf(f, "%d;", kb);
		fprintf(f, "\n");
	}
	fprintf(f, "Total %d;;;%d\n", Mgr->m_Surfaces.GetSize(), TotalKB);


	fclose(f);
	Game->LOG(0, "Texture Stats Dump completed.");
	Game->QuickMessage("Texture Stats Dump completed.");
}

//////////////////////////////////////////////////////////////////////////
float CBRenderSDL::GetAlignedUpscalingRatio(float ratio, float stepping)
{
	float newRatio = 1.0;

	// sort out unreasonable values
	if (stepping < 0.001)
	{
		return ratio;
	}

	while ((newRatio + stepping) < ratio)
	{
		// only increase newRatio until it is at most equal to the old one
		if ((newRatio + stepping) <= ratio)
		{
			newRatio += stepping;
		}
	}

	return newRatio;
}

//////////////////////////////////////////////////////////////////////////
float CBRenderSDL::GetAlignedDownscalingRatio(float ratio, float stepping)
{
	float newRatio = 1.0;

	// sort out unreasonable values
	if (stepping < 0.001)
	{
		return ratio;
	}

	while ((newRatio - stepping) > ratio)
	{
		// decrease until newRatio is equal or lower than ratio
		newRatio -= stepping;
	}
	return newRatio;
}

//////////////////////////////////////////////////////////////////////////
void CBRenderSDL::EnableKeyboard(int yCoord)
{
	SDL_StartTextInput();
	Game->LOG(0, "Starting text input!");
}

//////////////////////////////////////////////////////////////////////////
void CBRenderSDL::DisableKeyboard()
{
	SDL_StopTextInput();
	Game->LOG(0, "Stopping text input!");
}

//////////////////////////////////////////////////////////////////////////
void CBRenderSDL::GetPhysicalScreenBorderDimensions(RECT *rect)
{
	rect->left   = m_PhysicalBorderLeft;
	rect->top    = m_PhysicalBorderTop;
	rect->right  = m_PhysicalBorderRight;
	rect->bottom = m_PhysicalBorderBottom;
}

//////////////////////////////////////////////////////////////////////////
void CBRenderSDL::GetPhysicalScreenProperties(RECT *rect)
{
	// FIXME yes this is a misuse of the RECT STRUCT...
	rect->left   = m_PhysicalWidth;
	rect->top    = m_PhysicalHeight;
	rect->right  = m_PhysicalDensityX;
	rect->bottom = m_PhysicalDensityY;
}
