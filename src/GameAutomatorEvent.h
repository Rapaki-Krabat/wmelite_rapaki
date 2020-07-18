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

enum class GameAutomatorEventSource { NONE, ACTOR, SCENE, ENTITY, BUTTON, INTERACTIVE };

class GameAutomatorEvent : public CBScriptable
{
public:
	// from CAdActor::TurnTo(TDirection dir)
	GameAutomatorEvent(CBGame* inGame, GameAutomatorEventSource source, char* name, TDirection turntoDir) : 
		CBScriptable(inGame), m_Source(source), m_TurntoDir(turntoDir) {
		m_Name = new AnsiString(name); 
	}
	// from CAdActor::GoTo(int X, int Y, TDirection AfterWalkDir)
	GameAutomatorEvent(CBGame* inGame, GameAutomatorEventSource source, char* name, int X, int Y) :
		CBScriptable(inGame), m_Source(source), m_X(X), m_Y(Y) {
		m_Name = new AnsiString(name);
	}
	// from CAdObject::Talk(char *Text, char* Sound, DWORD Duration, char *Stances, TTextAlign Align)
	// CAdGame::ChangeScene(char *Filename, bool FadeIn)
	// CUIButton::Press()
	GameAutomatorEvent(CBGame* inGame, GameAutomatorEventSource source, char* name, char* text) :
		CBScriptable(inGame), m_Source(source) {
		m_Name = new AnsiString(name);
		m_Sentence = new AnsiString(text);
	}
	// from CBGame::ScSetProperty(char *Name, CScValue *Value)
	GameAutomatorEvent(CBGame* inGame, GameAutomatorEventSource source, char* name, bool interactive) :
		CBScriptable(inGame), m_Source(source), m_Interactive(interactive) {
		m_Name = new AnsiString(name);
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
	GameAutomatorEventSource m_Source;
	AnsiString* m_Name;
	AnsiString* m_Sentence = NULL;
	TDirection m_TurntoDir = DI_NONE;
	int m_X = 0;
	int m_Y = 0;
	bool m_Interactive = true;
};

#endif
