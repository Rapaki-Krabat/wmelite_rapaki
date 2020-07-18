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
#include "GameAutomator.h"

IMPLEMENT_PERSISTENT(GameAutomator, false);

//////////////////////////////////////////////////////////////////////////
GameAutomator::GameAutomator(CBGame* inGame) :CBObject(inGame)
{
	queuedEvents.clear();
	reportedEvents.clear();
}

//////////////////////////////////////////////////////////////////////////
GameAutomator::~GameAutomator()
{
	queuedEvents.clear();
	reportedEvents.clear();
}

//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
HRESULT GameAutomator::ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name)
{
	//////////////////////////////////////////////////////////////////////////
	// AddMouseButtonEvent
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "AddMouseButtonEvent") == 0)
	{
		Stack->CorrectParams(4);
		Sint32 XCoord = Stack->Pop()->GetInt(0);
		Sint32 YCoord = Stack->Pop()->GetInt(0);
		char* btn = Stack->Pop()->GetString();
		char* upDown = Stack->Pop()->GetString();

		Uint8 btnVal = SDL_BUTTON_LEFT;
		if (strcmp(btn, "right") == 0) btnVal = SDL_BUTTON_RIGHT;
		else if (strcmp(btn, "middle") == 0) btnVal = SDL_BUTTON_MIDDLE;

		Uint32 upDownType = SDL_MOUSEBUTTONDOWN;
		if (strcmp(upDown, "up") == 0) upDownType = SDL_MOUSEBUTTONUP;

		SDL_Event* evt = new SDL_Event();
		evt->type = upDownType;
		evt->button.x = XCoord;
		evt->button.y = YCoord;
		evt->button.button = btnVal;
		AddInputEvent(evt);

		Stack->PushNULL();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AddMouseMotionEvent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AddMouseMotionEvent") == 0)
	{
		Stack->CorrectParams(2);
		Sint32 XCoord = Stack->Pop()->GetInt(0);
		Sint32 YCoord = Stack->Pop()->GetInt(0);

		SDL_Event* evt = new SDL_Event();
		evt->type = SDL_MOUSEMOTION;
		evt->motion.x = XCoord;
		evt->motion.y = YCoord;
		AddInputEvent(evt);

		Stack->PushNULL();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AddFingerMotionEvent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AddFingerMotionEvent") == 0)
	{
		Stack->CorrectParams(0);

		SDL_Event* evt = new SDL_Event();
		evt->type = SDL_FINGERMOTION;
		AddInputEvent(evt);

		Stack->PushNULL();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AddKeyEvent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AddKeyEvent") == 0)
	{
		Stack->CorrectParams(1);
		SDL_Keycode code = Stack->Pop()->GetInt(0);

		SDL_Event* evt = new SDL_Event();
		evt->type = SDL_KEYDOWN;
		evt->key.keysym.sym = code;
		AddInputEvent(evt);

		Stack->PushNULL();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// ClearReportedEvent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "ClearReportedEvent") == 0)
	{
		Stack->CorrectParams(1);
		size_t pos = Stack->Pop()->GetInt(0);

		if (reportedEvents.size() > pos)
		{
			GameAutomatorEvent* repEvent = reportedEvents.at(pos);
			delete repEvent;
			reportedEvents.erase(reportedEvents.begin() + pos);
		}

		Stack->PushNULL();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// GetReportedEvent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "GetReportedEvent") == 0)
	{
		Stack->CorrectParams(1);
		size_t pos = Stack->Pop()->GetInt(0);

		if (reportedEvents.size() > pos)
		{
			GameAutomatorEvent* repEvent = reportedEvents.at(pos);
			Stack->PushNative(repEvent, true);
		}
		else
		{
			Stack->PushNULL();
		}
		return S_OK;
	}

	else return CBObject::ScCallMethod(Script, Stack, ThisStack, Name);
}

//////////////////////////////////////////////////////////////////////////
CScValue* GameAutomator::ScGetProperty(char *Name)
{
	m_ScValue->SetNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "Type") == 0) {
		m_ScValue->SetString("automator");
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Name
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Name") == 0) {
		m_ScValue->SetString("game_automator");
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// ReportedEventSize
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "ReportedEventSize") == 0)
	{
		m_ScValue->SetInt((int) reportedEvents.size());
		return m_ScValue;
	}

	else return CBObject::ScGetProperty(Name);
}

//////////////////////////////////////////////////////////////////////////
HRESULT GameAutomator::ScSetProperty(char *Name, CScValue *Value)
{
	return CBObject::ScSetProperty(Name, Value);
}


//////////////////////////////////////////////////////////////////////////
char* GameAutomator::ScToString()
{
	return "[automator]";
}

//////////////////////////////////////////////////////////////////////////
HRESULT GameAutomator::Persist(CBPersistMgr* PersistMgr) {

	CBObject::Persist(PersistMgr);

	queuedEvents.clear();

	return S_OK;
}

