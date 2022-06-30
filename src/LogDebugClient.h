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
* @file LogDebugClient.h
*
*/

#ifndef __WmeLogDebugClient_H__
#define __WmeLogDebugClient_H__

#include <cstdint>
#include "wintypes.h"
#include "wme_debugger.h"
#include "Logger.h"

class LogDebugClient:public IWmeDebugClient
{
public:
	LogDebugClient(Logger* logger);

	virtual bool OnGameInit() override;
	virtual bool OnGameShutdown() override;

	virtual bool OnGameTick() override;

	virtual bool OnLog(unsigned int ErrorCode, const char* Text) override;

	virtual bool OnScriptInit(IWmeDebugScript* Script) override;
	virtual bool OnScriptEventThreadInit(IWmeDebugScript* Script, IWmeDebugScript* ParentScript, const char* EventName) override;
	virtual bool OnScriptMethodThreadInit(IWmeDebugScript* Script, IWmeDebugScript* ParentScript, const char* MethodName) override;
	virtual bool OnScriptShutdown(IWmeDebugScript* Script) override;
	virtual bool OnScriptChangeLine(IWmeDebugScript* Script, int Line) override;
	virtual bool OnScriptChangeScope(IWmeDebugScript* Script, unsigned int ScopeID) override;
	virtual bool OnScriptShutdownScope(IWmeDebugScript* Script, unsigned int ScopeID) override;

	virtual bool OnVariableInit(EWmeDebuggerVariableType Type, IWmeDebugScript* Script, unsigned int ScopeID, IWmeDebugProp* Variable, const char* VariableName) override;
	virtual bool OnVariableChangeValue(IWmeDebugProp* Variable, IWmeDebugProp* Value) override;

	virtual bool OnScriptHitBreakpoint(IWmeDebugScript* Script, int Line) override;

	virtual ~LogDebugClient();
private:
	Logger* m_Logger;
};

#endif // __WmeLogDebugClient_H__
