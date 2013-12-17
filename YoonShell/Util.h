#pragma once
class Util
{
public:
	static bool IsSameCharacter ( wchar_t inputChar[], wchar_t comparison[] );
	static wchar_t* ArrToPtr(wchar_t param[]);
	static const int MAX_PATH_LEN = 100;
	static bool IsContainCharacter ( wchar_t inputChar[], wchar_t comparison[] );
private:
	Util(void);
	~Util(void);
};
