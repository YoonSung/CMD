// YoonShell.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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
	wprintf ( L"명령어 처리를 종료합니다\n" );
	getchar();
}

int _tmain(int argc, _TCHAR* argv[])
{
	init();
	while ( ps->CommandProcessing() );
	destroy();
	return 0;
}