#include "stdafx.h"
#include "Instruction.h"
#include "Util.h"
#include "afxinet.h"
#include <io.h>
#include <errno.h>

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
			cmdDIR(NULL);
		else if ( Util::IsSameCharacter( arr[1], L">" ) )
			cmdPWD(arr[2]);

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

void CInstruction::cmdDIR(wchar_t * fileName)
{
	bool isRedirection = false;
	wchar_t resultTxt[MAX_PATH];
	ZeroMemory(resultTxt, 0);

	if ( fileName != NULL )
		isRedirection = true;



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
		FILETIME lastWriteFileTime = findFileData.ftLastWriteTime;
		SYSTEMTIME stWriteUTC, stWriteLocal;
		FileTimeToSystemTime ( &lastWriteFileTime, & stWriteUTC );
		SystemTimeToTzSpecificLocalTime ( NULL, &stWriteUTC, &stWriteLocal );

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

		wprintf ( L"%4d-%02d-%02d   %s %02d:%02d   ", stWriteLocal.wYear, stWriteLocal.wMonth, stWriteLocal.wDay, noon, hour, stWriteLocal.wMinute);

		if (FILE_ATTRIBUTE_DIRECTORY & findFileData.dwFileAttributes )
		{

			wchar_t buf[100];
			wsprintf(buf, L"<DIR>                %s\n", findFileData.cFileName);

			if ( isRedirection )
			{
				wcsncat_s ( resultTxt, buf, sizeof(dirPath) );
			}
			else
			{
				//wprintf ( L"<DIR>                %s\n", findFileData.cFileName );
				wprintf ( L"%s", buf );
			}
			
			totalDirectoryNum +=1;
		} else {
			wprintf ( L"%20d %s\n", findFileData.nFileSizeLow, findFileData.cFileName ); 
			totalFileNum +=1;
		}
	} while ( FindNextFile (hFind, &findFileData) != 0 );

	wprintf ( L"               %d개 파일\n", totalFileNum);
	wprintf ( L"               %d개 디렉터리\n", totalDirectoryNum);

	//if ( isRedirection )
	//	;

	FindClose ( hFind );
}

void CInstruction::cmdLS(wchar_t * option)
{
	cmdDIR(NULL);
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
	wprintf(L"savefile : %s\n", resultTxt.GetString());
	wprintf(L"size : %d\n", resultTxt.GetLength()*sizeof(wchar_t));

	HANDLE hSaveFile = CreateFile( fileName, GENERIC_WRITE, 0 , NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if ( hSaveFile == INVALID_HANDLE_VALUE )
	{
		wprintf(L"File Create Error %s\n", fileName);
		return;
	}

	DWORD numOfByteWritten = 0;
	WriteFile(hSaveFile, resultTxt, resultTxt.GetLength()*sizeof(wchar_t), &numOfByteWritten, NULL);
	CloseHandle(hSaveFile);

	wprintf(L"Total file Data Size : %d, Total Written Size : %d \n\n", resultTxt.GetLength()*sizeof(wchar_t), numOfByteWritten);
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