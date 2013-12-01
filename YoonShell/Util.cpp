#include "stdafx.h"
#include "Util.h"


Util::Util(void)
{
}


Util::~Util(void)
{
}

bool Util::IsSameCharacter ( wchar_t inputChar[], wchar_t comparison[] )
{
	return ( _wcsicmp (inputChar, comparison) == 0 );
}

wchar_t* Util::ArrToPtr(wchar_t param[])
{
	return param;
}