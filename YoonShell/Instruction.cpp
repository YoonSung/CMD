#include "stdafx.h"
#include "Instruction.h"
#include "Util.h"

CInstruction::CInstruction ( void )
{
	m_Process = CProcess::getInstancePtr();
}

CInstruction::~CInstruction ( void )
{

}
void saveFile(CString resultTxt, wchar_t* fileName);

bool CInstruction::GetExecuteCmd ( wchar_t arr[][256] , int tokenNum )
{
	if ( Util::IsSameCharacter( arr[0], L"cd" ) )
	{
		cmdCD(arr, tokenNum);
	} else if ( Util::IsSameCharacter( arr[0], L"pwd" ) && tokenNum <= 3 ) {
		if ( tokenNum == 1)
			cmdPWD(NULL);
		else if ( Util::IsSameCharacter( arr[1], L">" ) )
			cmdPWD(arr[2]);

	} else if ( Util::IsSameCharacter( arr[0], L"dir" ) && tokenNum <= 3 ) {
		if ( tokenNum == 1)
			cmdDIR(NONE, NULL);
		else if ( Util::IsSameCharacter( arr[1], L">"))
			cmdDIR(REDIRECT, arr[2]);
		else if ( Util::IsContainCharacter( arr[1], L"*"))
			cmdDIR(ASTERISK, arr[1]);
		else
			wprintf(L"DIR명령어의 잘못된 사용입니다. 다시한번 확인해 주세요.\n");

	} else if ( Util::IsSameCharacter( arr[0], L"ls" ) && tokenNum == 1 ) {
		cmdLS(NULL);
	} else if ( Util::IsSameCharacter( arr[0], L"ps" ) && tokenNum == 1 ) {
		cmdPS(NULL);
	} else if ( Util::IsSameCharacter( arr[0], L"kill" ) && tokenNum == 2 ) {
		cmdKILL(arr[1]);
	} else if ( Util::IsSameCharacter( arr[0], L"md" ) && tokenNum == 2 ) {
		CreateDirectory ( arr[1], NULL );
	} else if ( Util::IsSameCharacter( arr[0], L"rd" ) && tokenNum == 2 ) {
		RemoveDirectory ( arr[1] );
	} else if ( Util::IsSameCharacter( arr[0], L"copy") && tokenNum <= 3 ) {
		cmdCOPY( arr[1], arr[2]);
	} else if ( Util::IsSameCharacter( arr[0], L"del") && tokenNum == 2 ) {
		cmdFDEL(arr[1]);
	} else if ( Util::IsSameCharacter( arr[0], L"cat") && tokenNum == 2 ) {
		cmdCAT(arr[1]);
	} else if ( Util::IsSameCharacter( arr[0], L"ren") && tokenNum <= 3 ) {
		cmdREN(arr[1], arr[2]);
	} else {
		return false;
	}

	return true;
}

void CInstruction::cmdCD(wchar_t arr[][256], int tokenNum)
{

	wchar_t * changePath;

	if ( tokenNum ==  2 ) {
		changePath = arr[1];
	}else {
		wchar_t * path;
		wchar_t tempPath [ MAX_PATH ];

		memset(tempPath, NULL, sizeof(tempPath));

		for ( int i = 1 ; i < tokenNum ; i++ )
		{
			wcscat_s(tempPath, arr[i]);
			//it is not a solution. if user input space twice, or first & last letter is space. There is no way to know.
			//it seems to be executed when command is parsed,
			wcscat_s(tempPath, L" ");
		}
		path = Util::ArrToPtr(tempPath);
		changePath = path;
	}
	
	if ( Util::IsSameCharacter ( changePath, L".." ) )
	{
		wchar_t tempPath [ Util::MAX_PATH_LEN ];
		GetCurrentDirectory( Util::MAX_PATH_LEN, tempPath );

		if ( wcslen(tempPath) == 3 )
			return;

		wcscat_s(tempPath, L"\\..");

		changePath = Util::ArrToPtr(tempPath);
	}

	m_Process->setCurrentDir(changePath);
}


void CInstruction::cmdPWD(wchar_t * fileName)
{
	wchar_t currentPath [ Util::MAX_PATH_LEN ];
	GetCurrentDirectory ( Util::MAX_PATH_LEN, currentPath );

	CString resultTxt = currentPath;


	if ( fileName == NULL)
	{
		wprintf(L"%s\n",resultTxt.GetString());
	}
	else
	{
		saveFile(resultTxt, fileName);
		wprintf(L"%s\n",resultTxt.GetString());
	}
}

void CInstruction::cmdDIR(OPTION cmdOption, wchar_t * paramFileName)
{
	CString Entire_ResultTxt;
	CString Line_ResultTxt;
	CString temp;

	CString paramFname = paramFileName;
	
	CString tokenList[MAX_STR_LEN];
	int tokenNum = 0;

	//memset
	for ( int i = 0 ; i < MAX_STR_LEN ; ++i )
	{
		tokenList[i] = L"NULL";
	}


	if ( cmdOption == ASTERISK )
	{
		int StringIndex = 0;
		int tokenIndex = 0;
		CString tempString = L"";

		//배열에 * 자신과 *사이의 인덱스를 입력하는 배열에 저장.
		wprintf(L"length : %d\n", paramFname.GetLength()-1);
		while ( StringIndex <= paramFname.GetLength()-1 )
		{
			wprintf(L"StringIndex : %d\n", StringIndex);
			CString temp = paramFname.GetAt(StringIndex);

			if ( temp != L"*" )
			{
				tempString.Append(temp);
			}
			else
			{
				if ( StringIndex != 0 )
				{
					tokenList[tokenIndex] = tempString;
					++tokenIndex;
				}
				
				tokenList[tokenIndex] = L"*"; 
				++tokenIndex;
				tempString = L"";
			}

			++StringIndex;
		}

		CString lastChar = paramFname.GetAt(paramFname.GetLength()-1);
		if ( lastChar != L"*" )
		{
			tokenList[tokenIndex] = tempString;
			tokenNum = tokenIndex+1;
		}
		else
		{
			tokenNum = tokenIndex;
		}

		/*
		// 테스트 코드 
		int listIndex = 0;
		while ( tokenList[listIndex] != L"NULL" )
		{
			wprintf(L"tokenList[%d] : %s\n", listIndex, tokenList[listIndex] );
			++listIndex;
		}
		wprintf(L"tokenNum = %d\n", tokenNum);
		*/
	}

	wchar_t dirPath [ MAX_PATH ];
	GetCurrentDirectory ( MAX_PATH, dirPath );
	wcsncat_s ( dirPath, L"\\*", 3 );

	WIN32_FIND_DATA findFileData ; 
	HANDLE hFind = FindFirstFile ( dirPath, &findFileData );

	if ( hFind == INVALID_HANDLE_VALUE )
	{
		wprintf(L"UnExpectedError Occur\n");
		return;
	}
	int totalFileNum = 0;
	int totalDirectoryNum = 0;

	do
	{
		//버퍼초기화
		Line_ResultTxt = L"";

		FILETIME lastWriteFileTime = findFileData.ftLastWriteTime;
		SYSTEMTIME stWriteUTC, stWriteLocal;
		FileTimeToSystemTime ( &lastWriteFileTime, & stWriteUTC );
		SystemTimeToTzSpecificLocalTime ( NULL, &stWriteUTC, &stWriteLocal );


		//cmdOption이 있을경우
		if ( cmdOption == ASTERISK )
		{
			CString compareString = findFileData.cFileName;


			//시작부 검사
			//첫번째 토큰이 *이 아닐 경우
			if ( tokenList[0] != L"*" )
			{
				//첫번째 토큰을 찾지 못하면 다음파일을 검색 (do~ while 문 continue)
				if ( compareString.Find(tokenList[0], 0) != 0 )
					continue;
			}
			//첫번째 토큰이 *일 경우
			else
			{
				//두번째 토큰을 찾아서
				int findIndex = compareString.Find(tokenList[1]);

				//만약 존재하지 않는다면 다음파일을 검색 (do~ while 문 continue)
				if ( findIndex == -1 )
					continue;
				
				//만약 존재한다면 문자열에서 토큰의 위치까지의 스트링값을 잘라서 나머지 부분들만을 compareString에 저장한다.
				//ex) as를 검색 -> ddasdddd 파일일 경우, 앞의 ddas는 없애고, dddd를 저장하는 것이다.
				compareString = compareString.Mid( findIndex+tokenList[1].GetLength(), compareString.GetLength() - findIndex );
				wprintf(L"compareString : %s, findIndex : %d\n", compareString, findIndex);
			}

			//중간부 검사
			bool isCheckAll = true;
			//마지막 토큰은 아래에서 검사. 그러므로  tokenNum-1까지 (index이므로 미포함)
			for (int i = 1 ; i < tokenNum - 1 ; ++i )
			{
				if ( tokenList[i] == L"*" )
				{
					continue;
				}
				else
				{
					int findIndex = compareString.Find(tokenList[i], 0);

					if ( findIndex == -1 )
					{
						isCheckAll = false;
						break;
					}
					compareString = compareString.Mid( findIndex+tokenList[1].GetLength(), compareString.GetLength() - findIndex );
					wprintf(L"compareString : %s, findIndex : %d\n", compareString, findIndex);
				}
			}

			//중간부 검사때  for문을 사용하고 있으므로, boundary condition용 변수를 선언.
			if ( !isCheckAll )
				continue;


			//마지막 검사
			//만약 마지막토큰이 *이 아닐 경우
			if ( tokenList [ tokenNum ] != L"*" )
			{
				//중간부 검사때 제목을 다 사용했으므로 파일명을 다시 얻어온다.
				CString fileName = findFileData.cFileName;

				//파일명의 마지막 인덱스값
				int lastIndex = fileName.GetLength()-1;

				//마지막토큰의 길이
				int tokenLength = tokenList[ tokenNum-1 ].GetLength();

				//파일이름에서 마지막문자열을, 마지막토큰크기만큼 잘라서 저장한다.
				fileName = fileName.Mid( lastIndex - tokenLength+1, tokenLength );

				//테스트코드 wprintf(L"last check : %s\n", fileName);
				//substring한 파일이름고 마지막토큰과 비교해서 일치하지 않으면 다음파일을 검색 (do~ while 문 continue)
				if ( fileName.Find(tokenList[ tokenNum-1 ], 0 != 0 ) )
					continue;
			}
		}







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

		temp.Format(L"%4d-%02d-%02d   %s %02d:%02d   ", stWriteLocal.wYear, stWriteLocal.wMonth, stWriteLocal.wDay, noon, hour, stWriteLocal.wMinute);
		Line_ResultTxt.Append(temp);

		if (FILE_ATTRIBUTE_DIRECTORY & findFileData.dwFileAttributes )
		{
			temp.Format( L"<DIR>                %s\n", findFileData.cFileName );
			Line_ResultTxt.Append(temp);
			totalDirectoryNum +=1;
		} else {
			temp.Format( L"%20d %s\n", findFileData.nFileSizeLow, findFileData.cFileName ); 
			Line_ResultTxt.Append(temp);
			totalFileNum +=1;
		}

		//한줄 결과를 최종결과값에 저장.
		Entire_ResultTxt.Append(Line_ResultTxt);
	} while ( FindNextFile (hFind, &findFileData) != 0 );

	temp.Format( L"               %d개 파일\n", totalFileNum);
	Entire_ResultTxt.Append(temp);

	temp.Format( L"               %d개 디렉터리\n", totalDirectoryNum);
	Entire_ResultTxt.Append(temp);


	if ( cmdOption == REDIRECT )
	{
		saveFile(Entire_ResultTxt, paramFileName);
	}
	else
	{
		wprintf(Entire_ResultTxt);
	}	

	FindClose ( hFind );
}

void CInstruction::cmdLS(wchar_t * option)
{
	cmdDIR(NONE, NULL);
}

void CInstruction::cmdPS(wchar_t * option)
{
	m_Process->ShowCurrentProcessList();
}

void CInstruction::cmdKILL(wchar_t * target)
{
	m_Process->KillProcess(target);
}

void saveFile(CString resultTxt, wchar_t* fileName)
{
	//wprintf(L"savefile : %s\n", resultTxt.GetString());
	//wprintf(L"size : %d\n", resultTxt.GetLength()*sizeof(wchar_t));

	HANDLE hSaveFile = CreateFile( fileName, GENERIC_WRITE, 0 , NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if ( hSaveFile == INVALID_HANDLE_VALUE )
	{
		wprintf(L"File Create Error %s\n", fileName);
		return;
	}

	DWORD numOfByteWritten = 0;
	WriteFile(hSaveFile, resultTxt, resultTxt.GetLength()*sizeof(wchar_t), &numOfByteWritten, NULL);
	CloseHandle(hSaveFile);

	//wprintf(L"Total file Data Size : %d, Total Written Size : %d \n\n", resultTxt.GetLength()*sizeof(wchar_t), numOfByteWritten);
}

bool CInstruction::IsFileExist (wchar_t * path, wchar_t* fileName)
{
	wchar_t dirPath [ MAX_PATH ];
	ZeroMemory( dirPath, MAX_PATH );
	wcsncat_s ( dirPath, path, sizeof(dirPath) );
	wcsncat_s ( dirPath, L"\\*", 3 );

	WIN32_FIND_DATA findFileData ; 
	HANDLE hFind = FindFirstFile ( dirPath, &findFileData );

	bool isResourceFileExist = false, isTargetFileExist = false;

	if ( hFind == INVALID_HANDLE_VALUE )
	{
		wprintf(L"경로를 다시 확인해 주세요\n");
		return false;
	}
	do
	{
		if ( Util::IsSameCharacter ( findFileData.cFileName, fileName ) )
		{
			if (FILE_ATTRIBUTE_DIRECTORY & findFileData.dwFileAttributes)
			{
				wprintf(L"디렉터리는 복사할 수 없습니다.\n");
				return false;
			}
			return true;
		}
	} while ( FindNextFile (hFind, &findFileData) != 0 );

	return false;
}

void CInstruction::cmdCOPY(wchar_t* source, wchar_t* target)
{
	if ( target == NULL || source == NULL)
	{
		wprintf(L"명령어 사용이 잘못되었습니다.\ns");
		return;
	}

	wchar_t * sourceFileName = source;
	wchar_t * targetFileName = target;

	wchar_t sourcePath [ MAX_PATH ];
	wchar_t targetPath [ MAX_PATH ];

	//if source, target is path expression
	
	//parsing, 
	//save a name
	//sourceName = parsingSourceFileName;
	//targetName = parsingTargetFileName;
	//save a path
	//sourcePath = parsingSourcePath;       // find \ and add \ again (\\)
	//targetPath = parsingTargetPath;
	
	
	
	//else
	GetCurrentDirectory ( MAX_PATH, sourcePath );
	GetCurrentDirectory ( MAX_PATH, targetPath );

	if ( IsFileExist(sourcePath, sourceFileName) )
	{
		if ( !IsFileExist(targetPath, targetFileName) )
		{
			wcsncat_s ( sourcePath, L"\\", 2 );
			wcsncat_s ( sourcePath, sourceFileName, sizeof(sourceFileName)+1 );

			wcsncat_s ( targetPath, L"\\", 2 );
			wcsncat_s ( targetPath, targetFileName, sizeof(targetFileName)+1 );

			CopyFile( sourcePath, targetPath, NULL);
		}	
		else 
		{
			wcsncat_s ( sourcePath, L"\\", 2 );
			wcsncat_s ( sourcePath, sourceFileName, sizeof(sourceFileName)+1 );

			wprintf(L"is file exist\n");
			
			wchar_t fileName[ MAX_PATH ];
			ZeroMemory(fileName, MAX_PATH);
			wcsncat_s ( fileName, targetFileName, MAX_PATH );

			wchar_t * addName = L"_Copy";


			while ( IsFileExist(targetPath, fileName) )
			{
				wprintf(L"add name \n");
				wcsncat_s ( fileName, addName, sizeof(addName)+1 );
				wprintf(L"fileName : %s\n", fileName);
			}
			wcsncat_s ( targetPath, L"\\", 2 );
			wcsncat_s ( targetPath, fileName, sizeof(fileName)+1 );


			wprintf(L"targetPath : %s\n", targetPath);
			CopyFile( sourcePath, targetPath, NULL);
		}
	}
}

void CInstruction::cmdFDEL(wchar_t * target)
{
	wchar_t path [ MAX_PATH ];
	GetCurrentDirectory ( MAX_PATH, path );

	wcsncat_s ( path, L"\\", 3 );
	wcsncat_s ( path, target, sizeof(target)+1);

	int result = _wremove(path);

	switch (result)
	{
		case ENOENT :
			wprintf(L"파일삭제를 실패했습니다.\n");
			break;
		case EACCES :
			wprintf(L"권한이 없습니다.\n");
			break;
		case EISDIR :
			wprintf(L"폴더는 삭제할 수 없습니다.\n");
			break;
	}
}

void CInstruction::cmdREN(wchar_t* srcName, wchar_t* destName)
{
	if ( Util::IsSameCharacter(srcName, destName) )
	{
		wprintf_s(L"rename 전, 후의 이름이 같습니다! 전기를 아낍시다!\n");
		return;
	}

	if (!MoveFile(srcName, destName))
	{
		wprintf_s(L"Error! CODE : %d \n", GetLastError());
		switch(GetLastError())
		{
		case 2:
			wprintf_s(L"명령어 인자가 잘못되었습니다.\n");
			break;
		}
	}

	return;
}

void CInstruction::cmdCAT(wchar_t * target)
{
	wchar_t path [ MAX_PATH ];
	GetCurrentDirectory ( MAX_PATH, path );

	wcsncat_s ( path, L"\\", 3 );
	wcsncat_s ( path, target, sizeof(path));

	HANDLE hLoadFile = CreateFile ( path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if ( hLoadFile == INVALID_HANDLE_VALUE ){
		wprintf(L"File Open Error %s\n", path);
		return;
	}
	wchar_t fileData[100];

	DWORD numOfByteRead = 0;
	ReadFile ( hLoadFile, &fileData, sizeof(fileData), &numOfByteRead, NULL);
	
	fileData[numOfByteRead/sizeof(wchar_t)] = 0;

	wprintf(L"%s\n", fileData);

	CloseHandle(hLoadFile);
}