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
#include "BRenderer.h"

#ifdef __WIN32__
#define _USE_MATH_DEFINES // for C++
#endif
#include <math.h>

//////////////////////////////////////////////////////////////////////
CBRenderer::CBRenderer(CBGame* inGame):CBBase(inGame)
{
	m_Instance = NULL;
	m_Window = NULL;
	m_ClipperWindow = NULL;
	m_Active = false;
	m_Ready = false;
	m_Windowed = true;
	m_ForceAlphaColor = 0x00;

	m_Width = m_Height = m_BPP = 0;
	CBPlatform::SetRectEmpty(&m_MonitorRect);

	m_RealWidth = m_RealHeight = 0;
	m_DrawOffsetX = m_DrawOffsetY = 0;
	m_RealDensityX = m_RealDensityY = 0;

	m_MaxObjectDistanceX = m_MaxObjectDistanceY = 0;
}


//////////////////////////////////////////////////////////////////////
CBRenderer::~CBRenderer()
{
	DeleteRectList();
	UnclipCursor();
}


//////////////////////////////////////////////////////////////////////
void CBRenderer::InitLoop()
{
	DeleteRectList();
}


//////////////////////////////////////////////////////////////////////
CBObject* CBRenderer::GetObjectAt(int X, int Y)
{
	LONG bestDistance = 0;
	CBActiveRect *bestObject = NULL;
	LONG bestOuterDistance = 0;
	CBActiveRect *bestOuterObject = NULL;
	POINT point;
	point.x = X;
	point.y = Y;

	for(int i=m_RectList.GetSize()-1; i>=0; i--)
	{
		if(CBPlatform::PtInRect(&m_RectList[i]->m_Rect, point))
		{
			if(m_RectList[i]->m_Precise)
			{
				// frame
				if(m_RectList[i]->m_Frame)
				{
					int XX = (int)((m_RectList[i]->m_Frame->m_Rect.left + X - m_RectList[i]->m_Rect.left + m_RectList[i]->m_OffsetX)/(float)((float)m_RectList[i]->m_ZoomX/(float)100));
					int YY = (int)((m_RectList[i]->m_Frame->m_Rect.top  + Y - m_RectList[i]->m_Rect.top  + m_RectList[i]->m_OffsetY) /(float)((float)m_RectList[i]->m_ZoomY/(float)100));

					if(m_RectList[i]->m_Frame->m_MirrorX)
					{
						int Width = m_RectList[i]->m_Frame->m_Rect.right - m_RectList[i]->m_Frame->m_Rect.left;
						XX = Width - XX;
					}

					if(m_RectList[i]->m_Frame->m_MirrorY)
					{
						int Height = m_RectList[i]->m_Frame->m_Rect.bottom - m_RectList[i]->m_Frame->m_Rect.top;
						YY = Height - YY;
					}

					if(!m_RectList[i]->m_Frame->m_Surface->IsTransparentAt(XX, YY)) return m_RectList[i]->m_Owner;
				}
				// region
				else if(m_RectList[i]->m_Region)
				{
					if(m_RectList[i]->m_Region->PointInRegion(X+m_RectList[i]->m_OffsetX, Y+m_RectList[i]->m_OffsetY)) return m_RectList[i]->m_Owner;
				}

				if ((m_RectList[i]->m_Frame) || (m_RectList[i]->m_Region))
				{
					// not a precise match but inside the defined rect --> possible candidate for later
					if (bestObject == NULL)
					{
						// first object candidate, remember values
						bestObject = m_RectList[i];
						bestDistance = CBPlatform::DistanceRectMiddle(&m_RectList[i]->m_Rect, point);

						// Game->LOG(0, "New near inside object candidate: '%s', dist=%d", bestObject->m_Owner->m_Name, bestDistance);
					}
					else
					{
						LONG newDistance = CBPlatform::DistanceRectMiddle(&m_RectList[i]->m_Rect, point);
						if (newDistance < bestDistance)
						{
							bestObject = m_RectList[i];
							bestDistance = newDistance;

							// Game->LOG(0, "Better near inside object candidate: '%s', dist=%d", bestObject->m_Owner->m_Name, bestDistance);
						}
					}
				}
			}
			else return m_RectList[i]->m_Owner;
		}
		else
		{
			if ((m_RectList[i]->m_Frame) || (m_RectList[i]->m_Region))
			{
				// as long as there is no better "inner" object, remember "outer" objects as well
				if (bestObject == NULL)
				{
					if (bestOuterObject == NULL)
					{
						// first object candidate, remember values
						bestOuterObject = m_RectList[i];
						bestOuterDistance = CBPlatform::DistanceRectMiddle(&m_RectList[i]->m_Rect, point);

						// Game->LOG(0, "New near outside object candidate: '%s', dist=%d", bestOuterObject->m_Owner->m_Name, bestOuterDistance);
					}
					else
					{
						LONG newDistance = CBPlatform::DistanceRectMiddle(&m_RectList[i]->m_Rect, point);
						if (newDistance < bestOuterDistance)
						{
							bestOuterObject = m_RectList[i];
							bestOuterDistance = newDistance;

							// Game->LOG(0, "Better near outside object candidate: '%s', dist=%d", bestOuterObject->m_Owner->m_Name, bestOuterDistance);
						}
					}
				}
			}
		}
	}
	
	if ((m_MaxObjectDistanceX > 0) || (m_MaxObjectDistanceY > 0))
	{
		LONG maxDistance = (LONG) sqrt((float) ((m_MaxObjectDistanceX * m_MaxObjectDistanceX) + (m_MaxObjectDistanceY * m_MaxObjectDistanceY)));

		// use the "outer" object if no "inner" object was found
		if ((bestObject == NULL) && (bestOuterObject != NULL))
		{
			bestObject = bestOuterObject;
			bestDistance = bestOuterDistance;
		}

		if (bestObject != NULL)
		{
			// Game->LOG(0, "Final near object candidate: '%s', dist=%d", bestObject->m_Owner->m_Name, bestDistance);

			if(bestObject->m_Frame)
			{
				int XX = (int)((bestObject->m_Frame->m_Rect.left + X - bestObject->m_Rect.left + bestObject->m_OffsetX)/(float)((float)bestObject->m_ZoomX/(float)100));
				int YY = (int)((bestObject->m_Frame->m_Rect.top  + Y - bestObject->m_Rect.top  + bestObject->m_OffsetY) /(float)((float)bestObject->m_ZoomY/(float)100));

				if(bestObject->m_Frame->m_MirrorX)
				{
					int Width = bestObject->m_Frame->m_Rect.right - bestObject->m_Frame->m_Rect.left;
					XX = Width - XX;
				}

				if(bestObject->m_Frame->m_MirrorY)
				{
					int Height = bestObject->m_Frame->m_Rect.bottom - bestObject->m_Frame->m_Rect.top;
					YY = Height - YY;
				}

				for (int xOff = -m_MaxObjectDistanceX; xOff <= m_MaxObjectDistanceX; xOff++)
				{
					double factor = ((double) xOff) / ((double) m_MaxObjectDistanceX);
					factor = cos(M_PI_2 * factor);
					int roundedFactor = (int) (((double) m_MaxObjectDistanceY) * factor);
					for (int yOff = -roundedFactor; yOff <= roundedFactor; yOff++)
					{
						if(!bestObject->m_Frame->m_Surface->IsTransparentAt(XX + xOff, YY + yOff)) return bestObject->m_Owner;
					}
				}
			}
			// region
			else if(bestObject->m_Region)
			{
				for (int xOff = -m_MaxObjectDistanceX; xOff <= m_MaxObjectDistanceX; xOff++)
				{
					double factor = ((double) xOff) / ((double) m_MaxObjectDistanceX);
					factor = cos(M_PI_2 * factor);
					int roundedFactor = (int) (((double) m_MaxObjectDistanceY) * factor);
					for (int yOff = -roundedFactor; yOff <= roundedFactor; yOff++)
					{
						if(bestObject->m_Region->PointInRegion(X+bestObject->m_OffsetX + xOff, Y+bestObject->m_OffsetY + yOff)) return bestObject->m_Owner;
					}
				}
			}
		}
		else
		{
			// Game->LOG(0, "No near object candidate found!");
		}
	}

	return (CBObject*)NULL;
}


//////////////////////////////////////////////////////////////////////////
void CBRenderer::DeleteRectList()
{
	for(int i=0; i<m_RectList.GetSize(); i++)
	{
		delete m_RectList[i];
	}
	m_RectList.RemoveAll();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::SwitchFullscreen()
{
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::Flip()
{
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::InitRenderer(int width, int height, bool windowed, float upScalingRatioStepping, float downScalingRatioStepping, bool pixelPerfectRendering, const AnsiString& renderingHint, bool vsync)
{
	return E_FAIL;
}

/////////////////////////////////////////////////////
void CBRenderer::WindowResized(int x, int y)
{
}

//////////////////////////////////////////////////////////////////////
void CBRenderer::OnWindowChange()
{
}

///////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::EraseBackground()
{
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::Fill(BYTE r, BYTE g, BYTE b, RECT* rect)
{
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::WindowedBlt()
{
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::Setup2D(bool Force)
{
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::SetupLines()
{
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::DrawLine(int X1, int Y1, int X2, int Y2, DWORD Color)
{
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::DrawRect(int X1, int Y1, int X2, int Y2, DWORD Color, int Width)
{
	for(int i=0; i<Width; i++)
	{
		DrawLine(X1+i, Y1+i, X2-i,   Y1+i,   Color); // up
		DrawLine(X1+i, Y2-i, X2-i+1, Y2-i, Color); // down

		DrawLine(X1+i, Y1+i, X1+i, Y2-i,   Color); // left
		DrawLine(X2-i, Y1+i, X2-i, Y2-i+1, Color); // right
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::Fade(WORD Alpha)
{
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::FadeToColor(DWORD Color, RECT* rect)
{
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::SetViewport(int left, int top, int right, int bottom)
{
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::SetScreenViewport()
{
	return SetViewport(m_DrawOffsetX, m_DrawOffsetY, m_Width + m_DrawOffsetX, m_Height + m_DrawOffsetY);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::SetViewport(RECT* Rect)
{
	return SetViewport(	Rect->left + m_DrawOffsetX,
						Rect->top + m_DrawOffsetY,
						Rect->right + m_DrawOffsetX,
						Rect->bottom + m_DrawOffsetY);
}


//////////////////////////////////////////////////////////////////////////
CBImage* CBRenderer::TakeScreenshot()
{
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::ClipCursor()
{
	/*
	if(!m_Windowed)
	{
		RECT rc;
		GetWindowRect(m_Window, &rc);

		// if "maintain aspect ratio" is in effect, lock mouse to visible area
		rc.left = m_DrawOffsetX;
		rc.top = m_DrawOffsetY;
		rc.right = rc.left + m_Width;
		rc.bottom = rc.top + m_Height;

		::ClipCursor(&rc);
	}
	 */
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBRenderer::UnclipCursor()
{
	/*
	if(!m_Windowed) ::ClipCursor(NULL);
	*/
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CBRenderer::PointInViewport(POINT* P)
{
	if(P->x < m_DrawOffsetX) return false;
	if(P->y < m_DrawOffsetY) return false;
	if(P->x > m_DrawOffsetX + m_Width) return false;
	if(P->y > m_DrawOffsetY + m_Height) return false;

	return true;
}
