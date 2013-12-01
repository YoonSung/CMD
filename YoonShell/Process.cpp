#include "stdafx.h"
#include <stdlib.h>
#include <locale.h>
#include <Windows.h>
#include <TlHelp32.h>
#include "Process.h"
#include "Util.h"
#include "Instruction.h"

const wchar_t* ERROR_CMD = L"'%s'는 실행할 수 있는 프로그램이 아닙니다. \n";
const wchar_t* SEPS = L" ,\t\n";

CProcess* CProcess::cProcess;
CInstruction* m_instruction;

CProcess::CProcess()
{
	_wsetlocale ( LC_ALL , L"Korean" );
	setCurrentDir ( L"c:\\windows" );
}

CProcess* CProcess::getInstancePtr()
{
		if ( cProcess == nullptr )
		{
			cProcess = new CProcess();
			m_instruction = new CInstruction();
		}
		return cProcess;
}

void CProcess::releaseInstancePtr()
{
	delete cProcess;
}

CProcess::~CProcess()
{
	delete m_instruction;
}


void CProcess::setCurrentDir ( wchar_t* path )
{
	int result = SetCurrentDirectory ( path );

	if ( result == 0 )
	{
		wprintf ( L"지정된 경로를 찾을 수 없습니다.\n" );
	}
	else
	{
		wchar_t tempPath[Util::MAX_PATH_LEN];
		GetCurrentDirectory( Util::MAX_PATH_LEN, tempPath );
		wcscpy_s ( m_currentDir, Util::ArrToPtr ( tempPath ) );
	}
}

int CProcess::Create ( wchar_t command[] )
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	ZeroMemory ( &si, sizeof( si ) );
	si.cb = sizeof(si);
	ZeroMemory ( &pi, sizeof( pi ) );

	if (!CreateProcess ( NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi ) )
	{
		
		wprintf ( ERROR_CMD, command );
		//For Developer
		//wprintf ( L"CreateProcess failed (%d). \n", GetLastError() );
		return 0;
	}

	//WaitForSingleObject ( pi.hProcess, INFINITE );

	CloseHandle ( pi.hProcess );
	CloseHandle ( pi.hThread );

	return 0;
}

bool CProcess::CommandProcessing()
{
	wprintf ( L"Yoonsung-Shell : %s >> ", m_currentDir );
	_getws_s ( m_GCommandString );

	if ( Util::IsSameCharacter( m_GCommandString, L"exit" ) )
		return false;

	wchar_t m_GCommandString_Copy[MAX_STR_LEN];
	wcscpy_s (m_GCommandString_Copy , m_GCommandString);

	wchar_t * nextToken = NULL;
	int tokenNum = 0;
	wchar_t * token = wcstok_s ( m_GCommandString_Copy, SEPS, &nextToken );

	while ( NULL != token )
	{
		wcscpy_s ( m_GCommandTokenList[tokenNum++], token );
		token = wcstok_s ( NULL, SEPS, &nextToken );
	}
	
	bool exeCmd = m_instruction->GetExecuteCmd ( m_GCommandTokenList, tokenNum );
	
	if ( !exeCmd ) {
		Create ( m_GCommandString );
	}	

	return true;
}

int CProcess::ShowCurrentProcessList()
{
	return UseProcessList ( PS_SHOWLIST, NULL );
}

int CProcess::KillProcess ( wchar_t* target )
{
	return UseProcessList ( PS_KILL, target );
}

int CProcess::UseProcessList ( PS_COMMAND command, wchar_t* target )
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 );

	if ( hProcessSnap == INVALID_HANDLE_VALUE )
		return 0;
	  
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof ( PROCESSENTRY32 ) ;

	if ( !Process32First ( hProcessSnap, &pe32 ) )
	{
		wprintf( L"Process32First ERROR\n" );
		CloseHandle( hProcessSnap );
		return 0;
	}
	

	if ( command == PS_SHOWLIST )
	{
		do
		{
			wprintf ( L"%d: %s\n", pe32.th32ProcessID , pe32.szExeFile );
		}
		while ( Process32Next ( hProcessSnap, &pe32 ) );
	}
	else if ( command == PS_KILL )
	{
		UINT ExitCode = 0;
		bool result = false;
		do
		{
			if ( !_wcsicmp ( pe32.szExeFile, target ) )
			{
				result = _KillProcess ( pe32.th32ProcessID,ExitCode );
			}
		}
		while ( Process32Next ( hProcessSnap, &pe32 ) );

		if ( result )
		{
			wprintf ( L"[SUCCESS] Process '%s' Killed\n", target );
		}
		else
		{
			wprintf ( L"[FAIL] Exceute Fail\n" );
		}
	}


	CloseHandle ( hProcessSnap );
	
	return 1;
}

bool CProcess::_KillProcess ( unsigned long dwPID, unsigned int  ExitCode ) 
{
	bool isKilled = false;

	HANDLE hProcess = OpenProcess ( PROCESS_TERMINATE, FALSE, dwPID );
	if ( hProcess == NULL )
		;
	else if ( TerminateProcess ( hProcess, ExitCode ) )
		isKilled = true;

	CloseHandle ( hProcess );

	return isKilled;
}