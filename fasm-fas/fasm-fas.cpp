#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#ifndef MAX_CMD
#define MAX_CMD	8192
#endif

void _tmain( int argc, TCHAR *argv[] )
{
	TCHAR prCommandLine[MAX_CMD+1] = {'\0'};
	TCHAR prModuleName[MAX_PATH+1] = {'\0'};
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	DWORD lpExitCode;

	// Setup the module name.
	ZeroMemory( prModuleName, sizeof(prModuleName) );
	if( !GetModuleFileName(NULL, prModuleName, MAX_PATH) ) {
		fprintf( stderr, "GetModuleFileName failed (%d).\n", GetLastError() );
		return;
	}
	if( !PathRemoveFileSpec(prModuleName) ) {
		fprintf( stderr, "PathRemoveFileSpec failed (%d).\n", GetLastError() );
		return;
	}
	StringCchCat( prModuleName, MAX_PATH, L"\\FAsm2.exe" );
	
	// Parse the commandline.
	ZeroMemory( prCommandLine, sizeof(prCommandLine) );
	for (int i = 0; i < argc; i++) {
		StringCchCat( prCommandLine, MAX_CMD, L"\"" );
		StringCchCat( prCommandLine, MAX_CMD, argv[i] );
		StringCchCat( prCommandLine, MAX_CMD, L"\" " );
	}
	StringCchCat( prCommandLine, MAX_CMD, L"-s \"PureBasic.fas\"" );

	// Setup the child process.
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO);

	// Start the child process. 
	if( !CreateProcess( prModuleName,   // Module name
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
		fprintf( stderr, "CreateProcess failed (%d).\n", GetLastError() );
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject( piProcInfo.hProcess, INFINITE );

	// Get the exit code of child process.
	if( !GetExitCodeProcess( piProcInfo.hProcess, &lpExitCode ) ) {
		fprintf( stderr, "GetExitCodeProcess failed (%d).\n", GetLastError() );
		return;
	}

	// Close process and thread handles. 
	CloseHandle( piProcInfo.hProcess );
	CloseHandle( piProcInfo.hThread );

	// Copy other files.
	CopyFile( L"PureBasic.asm", L"..\\PureBasic.asm", FALSE );
	CopyFile( L"PureBasic.fas", L"..\\PureBasic.fas", FALSE );
	CopyFile( L"PureBasic.obj", L"..\\PureBasic.obj", FALSE );

	// Exit this process.
	ExitProcess(lpExitCode);
}