#pragma once

#include "stdafx.h"

class CInstruction
{
public:
	CInstruction ( void );
	~CInstruction ( void );
	bool CheckCommand ( wchar_t* command, int paramSize );
};