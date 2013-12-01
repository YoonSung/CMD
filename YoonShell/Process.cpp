#include "stdafx.h"
#include <stdlib.h>
#include <locale.h>
#include <Windows.h>
#include <TlHelp32.h>
#include "Process.h"


const wchar_t* ERROR_CMD = L"'%s'는 실행할 수 있는 프로그램이 아닙니다. \n";
const wchar_t* SEPS = L" ,\t\n";

CProcess* CProcess::cProcess=nullptr;

CProcess::CProcess()
{
	_wsetlocale ( LC_ALL , L"Korean" );
	setCurrentDir ( L"c:\\windows" );
	m_instruction = new CInstruction();
}

CProcess* CProcess::getInstancePtr()
{
		if ( cProcess == nullptr )
		{
			cProcess = new CProcess();
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
		wprintf ( L"지정된 경로를 찾을 수 없습니다." );
	}
	else
	{
		m_currentDir = path;
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
	
	if ( !_wcsicmp ( m_GCommandString, L"exit" ) )
		return false;
	
	wchar_t * nextToken = NULL;
	int tokenNum = 0;
	wchar_t * token = wcstok_s ( m_GCommandString, SEPS, &nextToken );

	while ( NULL != token )
	{
		wcscpy_s ( m_GCommandTokenList[tokenNum++], token );
		token = wcstok_s ( NULL, SEPS, &nextToken );
	}

	bool checkResult = m_instruction->CheckCommand ( m_GCommandTokenList[0], tokenNum );

	if ( checkResult )
	{
		/*TODO :: 통과 이후 함수재호출 ( 상세스펙 확인 )
		
		//wchar_t* exeCmd = m_instruction->GetExecuteCmd ( m_GCommandTokenList );
		
		if ( exeCmd == nullptr )
			wprintf(L"명령어가 잘못되었네요");
			//명령어가 잘못되었네요
		else
			Create ( exeCmd );
		*/
		Create ( m_GCommandTokenList[0] );
	}	

	return true;
}

int CProcess::ShowCurrentProcessList()
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

	do
	{
		wprintf(L"%d: %s\n", pe32.th32ProcessID , pe32.szExeFile);
	}
	while ( Process32Next ( hProcessSnap, &pe32 ) );

	CloseHandle ( hProcessSnap );
	
	return 1;

}