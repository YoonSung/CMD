#pragma once

#include "stdafx.h"
#include "Process.h"

enum OPTION {
	ASTERISK,
	PIPELINE,
	REDIRECT,
	NONE,
};

class CInstruction
{
public:
	CInstruction ( void );
	~CInstruction ( void );
	bool CheckCommand ( wchar_t* command, int paramSize );
	bool GetExecuteCmd ( wchar_t arr[][256] , int tokenNum );
private:
	CProcess* m_Process;
	void cmdCD(wchar_t arr[][256], int tokenNum);
	void cmdPWD(wchar_t * fileName);
	void cmdCOPY(wchar_t* source, wchar_t* target);
	void CInstruction::cmdDIR(OPTION cmdOption, wchar_t * fileName);
	void cmdLS(wchar_t * option);
	void cmdPS(wchar_t * option);
	void cmdKILL(wchar_t * target);
	void cmdFDEL(wchar_t * target);
	void cmdCAT(wchar_t * target, OPTION cmdOption, wchar_t* additionalCmd);
	bool IsFileExist(wchar_t * path, wchar_t* fileName);
	void cmdREN(wchar_t* srcName, wchar_t* destName);
	void saveFile (CString resultTxt, wchar_t* fileName);
	void CatTextFile ( wchar_t* fileName, OPTION cmdOption, wchar_t* additionalCmd );
};