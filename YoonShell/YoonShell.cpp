// YoonShell.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Process.h"


CProcess* ps;


void init()
{
	ps = CProcess::getInstancePtr();
}

void destroy()
{
	CProcess::releaseInstancePtr();
	wprintf ( L"��ɾ� ó���� �����մϴ�\n" );
	getchar();
}

int _tmain(int argc, _TCHAR* argv[])
{
	init();
	while ( ps->CommandProcessing() );
	destroy();
	return 0;
}