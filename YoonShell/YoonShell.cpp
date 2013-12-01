// YoonShell.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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
	wprintf ( L"��ɾ� ó���� �����մϴ�\n" );
	
	return 0;
}