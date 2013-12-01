#pragma once

#include "stdafx.h"
#include "Instruction.h"

#define MAX_STR_LEN 256
#define CMD_TOKEN_NUM 10

class CProcess
{

public:

	wchar_t m_GCommandString[MAX_STR_LEN];
	wchar_t m_GCommandTokenList[CMD_TOKEN_NUM][MAX_STR_LEN];

	wchar_t* m_currentDir;
	static CProcess* cProcess;
	CInstruction* m_instruction;
	CProcess::CProcess();
	static CProcess* CProcess::getInstancePtr();
	static void CProcess::releaseInstancePtr();
	CProcess::~CProcess();
	void CProcess::setCurrentDir ( wchar_t* path );
	int CProcess::Create ( wchar_t command[] );
	bool CProcess::CommandProcessing();
	int CProcess::ShowCurrentProcessList();

private:
};