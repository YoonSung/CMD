#pragma once

#include "stdafx.h"
#include "Util.h"


#define MAX_STR_LEN 256
#define CMD_TOKEN_NUM 10

class CProcess
{
private:
	enum PS_COMMAND {
	PS_SHOWLIST,
	PS_KILL,
	};
public:
	static CProcess* getInstancePtr();
	static void releaseInstancePtr();
	wchar_t m_GCommandString[MAX_STR_LEN];
	wchar_t m_GCommandTokenList[CMD_TOKEN_NUM][MAX_STR_LEN];
	wchar_t m_currentDir[Util::MAX_PATH_LEN];
	static CProcess* cProcess;
	CProcess();
	~CProcess();
	void setCurrentDir ( wchar_t* path );
	int Create ( wchar_t command[] );
	bool CommandProcessing();
	int ShowCurrentProcessList();
	int UseProcessList ( PS_COMMAND command, wchar_t* target);
	int KillProcess(wchar_t* target);
	bool _KillProcess(unsigned long dwPID, unsigned int ExitCode);
};