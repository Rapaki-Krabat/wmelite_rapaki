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
* @file GameAutomator.cpp
*
*/

#include "dcgf.h"
#include "GameAutomator.h"

IMPLEMENT_PERSISTENT(GameAutomator, true);

//////////////////////////////////////////////////////////////////////////
GameAutomator::GameAutomator(CBGame* inGame) :CBObject(inGame)
{
	m_Enable = false;
	XOverride = 0;
	YOverride = 0;
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
void GameAutomator::AddAutomatorEvent(GameAutomatorEvent* evt) 
{ 
	if (m_Enable == true)
	{
		// if same event is already the most recent in the queue, just increase the repetition counter
		if (reportedEvents.size() > 0)
		{
			GameAutomatorEvent* lastEvt = reportedEvents.back();

			if (*lastEvt == *evt)
			{
				lastEvt->addRepetition();
				delete(evt);
				evt = NULL;
			}
		}

		// if event has not been deleted, it is not a duplicate and needs to be added
		if (evt != NULL)
		{
			reportedEvents.push_back(evt);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
/**
* Game Automator scripting interface: Methods.
*
* <b>All coordinates are in game dimensions (not screen, not points vs pixels...)!</b>
*/
HRESULT GameAutomator::ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name)
{
	//////////////////////////////////////////////////////////////////////////
	// AddMouseButtonEvent
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>AddMouseButtonEvent(int xCoordinate, int yCoordinate, string nameOfButton, string pressRelease);</b>
    * @param xCoordinate - Mouse/Touch event X coordinate
    * @param yCoordinate - Mouse/Touch event Y coordinate
    * @param nameOfButton - Either "left", "middle" or "right"
    * @param pressRelease - Either "down" or "up"
    */
	if (strcmp(Name, "AddMouseButtonEvent") == 0)
	{
		Stack->CorrectParams(4);
		Sint32 XCoord = Stack->Pop()->GetInt(0);
		Sint32 YCoord = Stack->Pop()->GetInt(0);
		char* btn = Stack->Pop()->GetString();
		char* upDown = Stack->Pop()->GetString();

		XOverride = XCoord;
		YOverride = YCoord;

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
		if (m_Enable == true) AddInputEvent(evt);

		Stack->PushNULL();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AddMouseMotionEvent
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>AddMouseMotionEvent(int xCoordinate, int yCoordinate);</b>
    * @param xCoordinate - Mouse/Touch event X coordinate
    * @param yCoordinate - Mouse/Touch event Y coordinate
    */
	else if (strcmp(Name, "AddMouseMotionEvent") == 0)
	{
		Stack->CorrectParams(2);
		Sint32 XCoord = Stack->Pop()->GetInt(0);
		Sint32 YCoord = Stack->Pop()->GetInt(0);

		XOverride = XCoord;
		YOverride = YCoord;

		SDL_Event* evt = new SDL_Event();
		evt->type = SDL_MOUSEMOTION;
		evt->motion.x = XCoord;
		evt->motion.y = YCoord;
		if (m_Enable == true) AddInputEvent(evt);

		Stack->PushNULL();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AddFingerMotionEvent
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>AddFingerMotionEvent();</b>
    */
	else if (strcmp(Name, "AddFingerMotionEvent") == 0)
	{
		Stack->CorrectParams(0);

		SDL_Event* evt = new SDL_Event();
		evt->type = SDL_FINGERMOTION;
		if (m_Enable == true) AddInputEvent(evt);

		Stack->PushNULL();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AddKeyEvent
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>AddKeyEvent(int keycode);</b>
    * @param keycode - Keycode as defined in SDL_Keycode struct
    */
	else if (strcmp(Name, "AddKeyEvent") == 0)
	{
		Stack->CorrectParams(1);
		SDL_Keycode code = Stack->Pop()->GetInt(0);

		SDL_Event* evt = new SDL_Event();
		evt->type = SDL_KEYDOWN;
		evt->key.keysym.sym = code;
		if (m_Enable == true) AddInputEvent(evt);

		Stack->PushNULL();
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// ClearReportedEvent
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>ClearReportedEvent(int position);</b>
    * @param position - which engine event in the queue to delete (== 0 if omitted)
    */
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
    /**
    * @brief <b>GetReportedEvent(int position);</b>
    * @param position - which engine event in the queue to return (== 0 if omitted)
    */
	else if (strcmp(Name, "GetReportedEvent") == 0)
	{
		Stack->CorrectParams(1);
		size_t pos = Stack->Pop()->GetInt(0);

		if (reportedEvents.size() > pos)
		{
			GameAutomatorEvent* repEvent = new GameAutomatorEvent(*reportedEvents.at(pos));
			Stack->PushNative(repEvent, false);
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
/**
* Game Automator scripting interface: Properties.
*
*/
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
	// Enable
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Enable") == 0)
	{
		m_ScValue->SetBool(m_Enable);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// ReportedEventSize
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.ReportedEventSize (R/O)</b>
    * @param int - size of the engine event queue
    */
	else if (strcmp(Name, "ReportedEventSize") == 0)
	{
		m_ScValue->SetInt((int) reportedEvents.size());
		return m_ScValue;
	}

	else return CBObject::ScGetProperty(Name);
}

//////////////////////////////////////////////////////////////////////////
/**
* Game Automator scripting interface: Properties.
*
*/
HRESULT GameAutomator::ScSetProperty(char *Name, CScValue *Value)
{
	//////////////////////////////////////////////////////////////////////////
	// Enable
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.Enable (R/W)</b>
    * @param bool - enable (true) or disable (false) game automator
    */
	if (strcmp(Name, "Enable") == 0) {
		m_Enable = Value->GetBool();
		return S_OK;
	}

	else return CBObject::ScSetProperty(Name, Value);
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

