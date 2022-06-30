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
* @file LogDebugClient.cpp
*
*/

#include "LogDebugClient.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
LogDebugClient::LogDebugClient(Logger* logger)
{
	m_Logger = logger;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnGameInit()
{
	m_Logger->LOG(0, "%s", "LogDebugClient::OnGameInit() --> stub");
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnGameShutdown()
{
	m_Logger->LOG(0, "%s", "LogDebugClient::OnGameShutdown() --> stub");
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnGameTick()
{
	m_Logger->LOG(0, "%s", "LogDebugClient::OnGameTick() --> stub");
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnLog(unsigned int ErrorCode, const char* Text)
{
	// don't log here, many lines would be logged twice
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnScriptInit(IWmeDebugScript* Script)
{
	// m_Logger->LOG(0, "%s", "LogDebugClient::OnScriptInit() --> stub");
	char buf[2048];
	buf[0] = 0;
	strcat(buf, "Script init: ");
	strcat(buf, Script->DbgGetFilename());


	m_Logger->LOG(0, "%s", buf);
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnScriptEventThreadInit(IWmeDebugScript* Script, IWmeDebugScript* ParentScript, const char* EventName)
{
	// m_Logger->LOG(0, "%s", "LogDebugClient::OnScriptEventThreadInit() --> stub");
	char buf[2048];
	buf[0] = 0;
	strcat(buf, "Script event thread init: ");
	strcat(buf, Script->DbgGetFilename());
	strcat(buf, ", Event: ");
	strcat(buf, EventName);

	m_Logger->LOG(0, "%s", buf);
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnScriptMethodThreadInit(IWmeDebugScript* Script, IWmeDebugScript* ParentScript, const char* MethodName)
{
	// m_Logger->LOG(0, "%s", "LogDebugClient::OnScriptMethodThreadInit() --> stub");
	char buf[2048];
	buf[0] = 0;
	strcat(buf, "Script event thread init: ");
	strcat(buf, Script->DbgGetFilename());
	strcat(buf, ", Method: ");
	strcat(buf, MethodName);

	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnScriptShutdown(IWmeDebugScript* Script)
{
	m_Logger->LOG(0, "%s", "LogDebugClient::OnScriptShutdown() --> stub");
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnScriptChangeLine(IWmeDebugScript* Script, int Line)
{
	m_Logger->LOG(0, "%s", "LogDebugClient::OnScriptChangeLine() --> stub");
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnScriptChangeScope(IWmeDebugScript* Script, unsigned int ScopeID)
{
	m_Logger->LOG(0, "%s", "LogDebugClient::OnScriptChangeScope() --> stub");
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnScriptShutdownScope(IWmeDebugScript* Script, unsigned int ScopeID)
{
	m_Logger->LOG(0, "%s", "LogDebugClient::OnScriptShutdownScope() --> stub");
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnVariableInit(EWmeDebuggerVariableType Type, IWmeDebugScript* Script, unsigned int ScopeID, IWmeDebugProp* Variable, const char* VariableName)
{
	// m_Logger->LOG(0, "%s", "LogDebugClient::OnVariableInit() --> stub");
	char buf[2048];
	char tmpbuf[1024];
	int numProps;

	buf[0] = 0;
	switch(Type)
	{
	case WME_DBGVAR_GLOBAL:
		strcat(buf, "Global variable init: ");
		break;
	case WME_DBGVAR_SCRIPT:
		strcat(buf, "Script variable init: ");
		break;
	case WME_DBGVAR_SCOPE:
		strcat(buf, "Scope variable init: ");
		break;
	}
	strcat(buf, VariableName);
	strcat(buf,", Value=");
	switch(Variable->DbgGetType())
	{
	case WME_DBGPROP_NULL:
		strcat(buf, "NULL");
		break;
	case WME_DBGPROP_INT:
		tmpbuf[0] = 0;
		sprintf(tmpbuf, "%d", Variable->DbgGetValInt());
		strcat(buf, tmpbuf);
		break;
	case WME_DBGPROP_FLOAT:
		tmpbuf[0] = 0;
		sprintf(tmpbuf, "%.2f", Variable->DbgGetValFloat());
		strcat(buf, tmpbuf);
		break;
	case WME_DBGPROP_BOOL:
		strcat(buf, Variable->DbgGetValBool() ? "true" : "false");
		break;
	case WME_DBGPROP_STRING:
		strcat(buf, Variable->DbgGetValString());
		break;
	case WME_DBGPROP_OBJECT:
		strcat(buf, "OBJECT");
		break;
	case WME_DBGPROP_NATIVE:
		strcat(buf, "NATIVE");
		break;
	case WME_DBGPROP_UNKNOWN:
		strcat(buf, "UNKNOWN");
		break;
	}

	m_Logger->LOG(0, "%s", buf);

	numProps = Variable->DbgGetNumProperties();
	if (numProps > 0)
	{
		const char* propName;
		IWmeDebugProp* propValue;

		for (int i = 0; i < numProps; i++)
		{
			buf[0] = 0;

			strcat(buf, "Property: ");
			Variable->DbgGetProperty(i, &propName, &propValue);

			strcat(buf, propName);
			strcat(buf,", Value=");

			switch(propValue->DbgGetType())
			{
			case WME_DBGPROP_NULL:
				strcat(buf, "NULL");
				break;
			case WME_DBGPROP_INT:
				tmpbuf[0] = 0;
				sprintf(tmpbuf, "%d", propValue->DbgGetValInt());
				strcat(buf, tmpbuf);
				break;
			case WME_DBGPROP_FLOAT:
				tmpbuf[0] = 0;
				sprintf(tmpbuf, "%.2f", propValue->DbgGetValFloat());
				strcat(buf, tmpbuf);
				break;
			case WME_DBGPROP_BOOL:
				strcat(buf, propValue->DbgGetValBool() ? "true" : "false");
				break;
			case WME_DBGPROP_STRING:
				strcat(buf, propValue->DbgGetValString());
				break;
			case WME_DBGPROP_OBJECT:
				strcat(buf, "OBJECT");
				break;
			case WME_DBGPROP_NATIVE:
				strcat(buf, "NATIVE");
				break;
			case WME_DBGPROP_UNKNOWN:
				strcat(buf, "UNKNOWN");
				break;
			}

			m_Logger->LOG(0, "%s", buf);
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnVariableChangeValue(IWmeDebugProp* Variable, IWmeDebugProp* Value)
{
	m_Logger->LOG(0, "%s", "LogDebugClient::OnVariableChangeValue() --> stub");
	return false;
}

//////////////////////////////////////////////////////////////////////
bool LogDebugClient::OnScriptHitBreakpoint(IWmeDebugScript* Script, int Line)
{
	m_Logger->LOG(0, "%s", "LogDebugClient::OnScriptHitBreakpoint() --> stub");
	return false;
}

//////////////////////////////////////////////////////////////////////
LogDebugClient::~LogDebugClient()
{
	m_Logger = NULL;
}
