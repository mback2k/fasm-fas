#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#ifndef MAX_CMD
#define MAX_CMD		MAX_PATH * 4
#endif

void _tmain( int argc, TCHAR *argv[] )
{
	TCHAR prCommandLine[MAX_CMD] = {'\0'};
	TCHAR prModuleName[MAX_PATH] = {'\0'};
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;

	// Setup the module name.
	GetModuleFileName(NULL, prModuleName, MAX_PATH);
	PathRemoveFileSpec(prModuleName);
	StringCchCat(prModuleName, MAX_PATH, L"\\FAsm2.exe");
	
	// Parse the commandline.
	for (int argi = 0; argi < argc; argi++) {
		StringCchCat(prCommandLine, MAX_CMD, L"\"");
		StringCchCat(prCommandLine, MAX_CMD, argv[argi]);
		StringCchCat(prCommandLine, MAX_CMD, L"\" ");
	}
	StringCchCat(prCommandLine, MAX_CMD, L"-s \"PureBasic.fas\"");

	// Setup the child process.
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	siStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Start the child process. 
	if( !CreateProcess( prModuleName,   // No module name (use command line)
		prCommandLine,  // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&siStartInfo,   // Pointer to STARTUPINFO structure
		&piProcInfo )   // Pointer to PROCESS_INFORMATION structure
	) {
		printf( "CreateProcess failed (%d).\n", GetLastError() );
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject( piProcInfo.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( piProcInfo.hProcess );
	CloseHandle( piProcInfo.hThread );

	// Copy other files.
	CopyFile( L"PureBasic.asm", L"..\\PureBasic.asm", FALSE);
	CopyFile( L"PureBasic.fas", L"..\\PureBasic.fas", FALSE);
	CopyFile( L"PureBasic.obj", L"..\\PureBasic.obj", FALSE);
}