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
	void cmdCD(wchar_t arr[][256], int tokenNum);
	void cmdPWD(wchar_t * fileName);
	void cmdCOPY(wchar_t* source, wchar_t* target);
	void cmdDIR(wchar_t * fileName);
	void cmdLS(wchar_t * option);
	void cmdPS(wchar_t * option);
	void cmdKILL(wchar_t * target);
	void cmdFDEL(wchar_t * target);
	void cmdCAT(wchar_t * target);
	bool IsFileExist(wchar_t * path, wchar_t* fileName);
	//void saveFile (CString resultTxt, wchar_t* fileName);
};