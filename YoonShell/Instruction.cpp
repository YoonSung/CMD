#include "stdafx.h"
#include "Instruction.h"
#include "Process.h"

CProcess* m_Process;

struct cmd
{
	wchar_t* command;
	int MaxParamLength;
	//자료구조 bst 구현
};

CInstruction::CInstruction ( void )
{
}

CInstruction::~CInstruction ( void )
{

}

bool CInstruction::CheckCommand ( wchar_t* command, int paramSize )
{
	m_Process = CProcess::getInstancePtr();
	//자료구조 적용 후에는 
	if ( command == L"cd" && paramSize == 2 )
		return true;
	else if ( command == L"pwd" && paramSize == 2 )
		return true;
	else
		return true;

	return false;
}