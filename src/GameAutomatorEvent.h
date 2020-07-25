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

#ifndef __WmeGameAutomatorEvent_H__
#define __WmeGameAutomatorEvent_H__


#include "BScriptable.h"
#include "dctypes.h"	

enum class GameAutomatorEventSource { NONE, ACTOR, SCENE, ENTITY, BUTTON, SETPROPERTY };

enum class GameAutomatorEventType { NONE, GOTO, TURNTO, TALK, CHANGE, PRESS, INTERACTIVE };

class GameAutomatorEvent : public CBScriptable
{
public:
	// generic constructor, needs to be called by all specific ctors
	GameAutomatorEvent(CBGame* inGame, GameAutomatorEventSource source, GameAutomatorEventType type, char* name) :
		CBScriptable(inGame), m_Source(source), m_Type(type) {
		m_Name = new AnsiString(name);
	}
	// from CAdActor::TurnTo(TDirection dir)
	GameAutomatorEvent(CBGame* inGame, GameAutomatorEventSource source, GameAutomatorEventType type, char* name, TDirection turntoDir) :
		GameAutomatorEvent(inGame, source, type, name) {
		m_TurntoDir = turntoDir;
	}
	// from CAdActor::GoTo(int X, int Y, TDirection AfterWalkDir)
	GameAutomatorEvent(CBGame* inGame, GameAutomatorEventSource source, GameAutomatorEventType type, char* name, int X, int Y) :
		GameAutomatorEvent(inGame, source, type, name) {
		m_X = X;
		m_Y = Y;
	}
	// from CAdObject::Talk(char *Text, char* Sound, DWORD Duration, char *Stances, TTextAlign Align)
	// CAdGame::ChangeScene(char *Filename, bool FadeIn)
	// CUIButton::Press()
	GameAutomatorEvent(CBGame* inGame, GameAutomatorEventSource source, GameAutomatorEventType type, char* name, char* text) :
		GameAutomatorEvent(inGame, source, type, name) {
		m_Sentence = new AnsiString(text);
	}
	// from CBGame::ScSetProperty(char *Name, CScValue *Value)
	GameAutomatorEvent(CBGame* inGame, GameAutomatorEventSource source, GameAutomatorEventType type, char* name, bool interactive) :
		GameAutomatorEvent(inGame, source, type, name)  {
		m_Interactive = interactive;
	}
	~GameAutomatorEvent() {
		delete m_Name;
		if (m_Sentence != NULL) delete m_Sentence;
	}

	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();

private:
	const GameAutomatorEventSource m_Source;
	const GameAutomatorEventType   m_Type;
	AnsiString* m_Name;
	AnsiString* m_Sentence = NULL;
	TDirection m_TurntoDir = DI_NONE;
	int m_X = 0;
	int m_Y = 0;
	bool m_Interactive = true;
};

#endif
