// YoonShell.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Process.h"
#include <windows.h>
#include <process.h>

HANDLE mutex;
DWORD dwWaitResult;

int _tmain(int argc, _TCHAR* argv[])
{
	mutex = CreateMutex ( NULL, FALSE, _T("NamedMutex"));

	if ( 183 == GetLastError()) 
	{
		ReleaseMutex(mutex);
		CloseHandle(mutex);
		return -1;
	}
	
	CProcess* ps = CProcess::getInstancePtr();
	while ( ps->CommandProcessing() );
	CProcess::releaseInstancePtr();
	wprintf ( L"명령어 처리를 종료합니다\n" );
	
	return 0;
}