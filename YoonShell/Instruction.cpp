#include "stdafx.h"
#include "Instruction.h"
#include "Util.h"
#include "afxinet.h"
#include <io.h>
#include <errno.h>

CInstruction::CInstruction ( void )
{
	m_Process = CProcess::getInstancePtr();
}

CInstruction::~CInstruction ( void )
{

}

bool CInstruction::GetExecuteCmd ( wchar_t arr[][256] , int tokenNum )
{
	wchar_t* cmd = nullptr;

	if ( Util::IsSameCharacter( arr[0], L"cd" ) )
	{
		if ( tokenNum ==  2 ) {
			cmdCD(arr[1]);
		}else {
			wchar_t * path;
			wchar_t tempPath [ MAX_PATH ];

			memset(tempPath, NULL, sizeof(tempPath));

			for ( int i = 1 ; i < tokenNum ; i++ )
			{
				wcscat_s(tempPath, arr[i]);

				//it is not a solution. if user input space twice, or first & last letter is space. There is no way to know.
				wcscat_s(tempPath, L" ");
			}
			path = Util::ArrToPtr(tempPath);
			cmdCD(path);
		}
	} else if ( Util::IsSameCharacter( arr[0], L"pwd" ) && tokenNum == 1 ) {
		cmdPWD();
	} else if ( Util::IsSameCharacter( arr[0], L"dir" ) && tokenNum == 1 ) {
		cmdDIR(NULL);
	} else if ( Util::IsSameCharacter( arr[0], L"ls" ) && tokenNum == 1 ) {
		cmdLS(NULL);
	} else if ( Util::IsSameCharacter( arr[0], L"ps" ) && tokenNum == 1 ) {
		cmdPS(NULL);
	} else if ( Util::IsSameCharacter( arr[0], L"kill" ) && tokenNum == 2 ) {
		cmdKILL(arr[1]);
	} else if ( Util::IsSameCharacter( arr[0], L"mkdir" ) && tokenNum == 2 ) {
		CreateDirectory ( arr[1], NULL );
	} else if ( Util::IsSameCharacter( arr[0], L"rmdir" ) && tokenNum == 2 ) {
		RemoveDirectory ( arr[1] );
	} else {
		return false;
	}

	return true;
}

void CInstruction::cmdCD(wchar_t * path)
{
	wchar_t * changePath = path;
	
	if ( Util::IsSameCharacter ( path, L".." ) )
	{
		wchar_t tempPath [ Util::MAX_PATH_LEN ];
		GetCurrentDirectory( Util::MAX_PATH_LEN, tempPath );

		if ( wcslen(tempPath) == 3 )
			return;

		wcscat_s(tempPath, L"\\..");

		changePath = Util::ArrToPtr(tempPath);
	}
	m_Process->setCurrentDir(path);
}


void CInstruction::cmdPWD()
{
	wchar_t currentPath [ Util::MAX_PATH_LEN ];
	GetCurrentDirectory ( Util::MAX_PATH_LEN, currentPath );
	wprintf(L"%s\n",currentPath);
}

int CInstruction::cmdDIR(wchar_t * option)
{
	wchar_t dirPath [ MAX_PATH ];
	GetCurrentDirectory ( MAX_PATH, dirPath );
	wcsncat_s ( dirPath, L"\\*", 3 );

	WIN32_FIND_DATA findFileData ; 
	HANDLE hFind = FindFirstFile ( dirPath, &findFileData );

	if ( hFind == INVALID_HANDLE_VALUE )
		return -1;

	int totalFileNum = 0;
	int totalDirectoryNum = 0;

	do
	{
		FILETIME lastWriteFileTime = findFileData.ftLastWriteTime;
		SYSTEMTIME stWriteUTC, stWriteLocal;
		FileTimeToSystemTime ( &lastWriteFileTime, & stWriteUTC );
		SystemTimeToTzSpecificLocalTime ( NULL, &stWriteUTC, &stWriteLocal );

		wchar_t * noon;
		int hour;
		
		if ( stWriteLocal.wHour > 12 )
		{
			noon = L"오후";
			hour = stWriteLocal.wHour - 12;
		}
		else
		{
			noon = L"오전";
			hour = stWriteLocal.wHour;
		}

		wprintf ( L"%4d-%02d-%02d   %s %02d:%02d   ", stWriteLocal.wYear, stWriteLocal.wMonth, stWriteLocal.wDay, noon, hour, stWriteLocal.wMinute);

		if (FILE_ATTRIBUTE_DIRECTORY & findFileData.dwFileAttributes )
		{
			wprintf ( L"<DIR>                %s\n", findFileData.cFileName );
			totalDirectoryNum +=1;
		} else {
			wprintf ( L"%20d %s\n", findFileData.nFileSizeLow, findFileData.cFileName ); 
			totalFileNum +=1;
		}
	} while ( FindNextFile (hFind, &findFileData) != 0 );

	wprintf ( L"               %d개 파일\n", totalFileNum);
	wprintf ( L"               %d개 디렉터리\n", totalDirectoryNum);


	FindClose ( hFind );
}

void CInstruction::cmdLS(wchar_t * option)
{
	cmdDIR(NULL);
}

void CInstruction::cmdPS(wchar_t * option)
{
	m_Process->ShowCurrentProcessList();
}

void CInstruction::cmdKILL(wchar_t * target)
{
	m_Process->KillProcess(target);
}