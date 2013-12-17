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

bool Util::IsContainCharacter ( wchar_t inputChar[], wchar_t comparison[] )
{
	std::wstring param = inputChar;
	bool result = false;

	if ( param.find( comparison, 0 ) != std::wstring::npos )
		result = true;

	return result;
}

wchar_t* Util::ArrToPtr(wchar_t param[])
{
	return param;
}