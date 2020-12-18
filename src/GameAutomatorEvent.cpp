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
* @file GameAutomatorEvent.cpp
*
*/

#include "dcgf.h"
#include "GameAutomatorEvent.h"

//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
HRESULT GameAutomatorEvent::ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name)
{
	return CBScriptable::ScCallMethod(Script, Stack, ThisStack, Name);
}

//////////////////////////////////////////////////////////////////////////
/**
* Game Automator event scripting interface: Properties.
*
*/
CScValue* GameAutomatorEvent::ScGetProperty(char *Name)
{
	m_ScValue->SetNULL();

	//////////////////////////////////////////////////////////////////////////
	// Source
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.Source (R/O)</b>
    * @param string - lower case name of the ::GameAutomatorEventSource enum
    */
	if (strcmp(Name, "Source") == 0)
	{
		switch (m_Source)
		{
		case GameAutomatorEventSource::NONE:
			m_ScValue->SetString("none");
			break;
		case GameAutomatorEventSource::ACTOR:
			m_ScValue->SetString("actor");
			break;
		case GameAutomatorEventSource::SCENE:
			m_ScValue->SetString("scene");
			break;
		case GameAutomatorEventSource::ENTITY:
			m_ScValue->SetString("entity");
			break;
		case GameAutomatorEventSource::BUTTON:
			m_ScValue->SetString("button");
			break;
		case GameAutomatorEventSource::SETPROPERTY:
			m_ScValue->SetString("setproperty");
			break;
		case GameAutomatorEventSource::WINDOW:
			m_ScValue->SetString("window");
			break;
		}

		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.Type (R/O)</b>
    * @param string - lower case name of the ::GameAutomatorEventType enum
    */
	if (strcmp(Name, "Type") == 0)
	{
		switch (m_Type)
		{
		case GameAutomatorEventType::NONE:
			m_ScValue->SetString("none");
			break;
		case GameAutomatorEventType::GOTO:
			m_ScValue->SetString("goto");
			break;
		case GameAutomatorEventType::TURNTO:
			m_ScValue->SetString("turnto");
			break;
		case GameAutomatorEventType::TALK:
			m_ScValue->SetString("talk");
			break;
		case GameAutomatorEventType::CHANGE:
			m_ScValue->SetString("change");
			break;
		case GameAutomatorEventType::PRESS:
			m_ScValue->SetString("press");
			break;
		case GameAutomatorEventType::INTERACTIVE:
			m_ScValue->SetString("interactive");
			break;
		case GameAutomatorEventType::PLAYANIM:
			m_ScValue->SetString("playanim");
			break;
		case GameAutomatorEventType::TAKEITEM:
			m_ScValue->SetString("takeitem");
			break;
		case GameAutomatorEventType::GETRESPONSE:
			m_ScValue->SetString("getresponse");
			break;
		case GameAutomatorEventType::SYSEXCLUSIVE:
			m_ScValue->SetString("sysexclusive");
			break;
		}

		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// TurnToDir
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.TurnToDir (R/O)</b>
    * @param int - integer value of the direction of the ::TDirection command
    */
	else if (strcmp(Name, "TurnToDir") == 0)
	{
		m_ScValue->SetInt(static_cast<int>(m_TurntoDir));
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Name
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.Name (R/O)</b>
    * @param string - name of the object that generated the event
    */
	else if (strcmp(Name, "Name") == 0)
	{
		if (m_Name != NULL)
		{
			m_ScValue->SetString(m_Name->c_str());
		}
		else
		{
			m_ScValue->SetNULL();
		}
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Sentence
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.Sentence (R/O)</b>
    * @param string - the sentence that is being displayed / spoken
    */
	else if (strcmp(Name, "Sentence") == 0)
	{
		if (m_Sentence != NULL)
		{
			m_ScValue->SetString(m_Sentence->c_str());
		}
		else
		{
			m_ScValue->SetNULL();
		}
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// X
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.X (R/O)</b>
    * @param int - X coordinate
    */
	else if (strcmp(Name, "X") == 0)
	{
		m_ScValue->SetInt(m_X);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Y
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.Y (R/O)</b>
    * @param int - Y coordinate
    */
	else if (strcmp(Name, "Y") == 0)
	{
		m_ScValue->SetInt(m_Y);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Interactive
	//////////////////////////////////////////////////////////////////////////
    /**
    * @brief <b>.Interactive (R/O)</b>
    * @param bool - value of Game.Interactive
    */
	else if (strcmp(Name, "Interactive") == 0)
	{
		m_ScValue->SetBool(m_Interactive);
		return m_ScValue;
	}

	else return CBScriptable::ScGetProperty(Name);
}

//////////////////////////////////////////////////////////////////////////
HRESULT GameAutomatorEvent::ScSetProperty(char *Name, CScValue *Value)
{
	return CBScriptable::ScSetProperty(Name, Value);
}


//////////////////////////////////////////////////////////////////////////
char* GameAutomatorEvent::ScToString()
{
	return "[automatorevent]";
}
