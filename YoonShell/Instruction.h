#pragma once

#include "stdafx.h"
#include "Process.h"

class CInstruction
{
public:
	CInstruction ( void );
	~CInstruction ( void );
	bool CheckCommand ( wchar_t* command, int paramSize );
	bool GetExecuteCmd ( wchar_t arr[][256] , int tokenNum );
private:
	CProcess* m_Process;
	void cmdCD(wchar_t * path);
	void cmdPWD();
	int cmdDIR(wchar_t * option);
	void cmdLS(wchar_t * option);
	void cmdPS(wchar_t * option);
	void cmdKILL(wchar_t * target);
};