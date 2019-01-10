#include "StdAfx.h"
#include <TlHelp32.h>
#include "ProcInterface.h"
#include <fstream>


#define SendDebugMessageT(dbginfo, i)  do {\
	if (IsWindowVisible(g_hwndDebug)) \
	{\
		::PostMessage(g_hwndDebug, MSG_APPEND_ADB_CMD, \
			(WPARAM)(new CString(dbginfo)), i);\
	}\
} while (0)

#define SendDebugMessage(dbginfo) SendDebugMessageT(dbginfo, 0)

extern HWND g_hwndDebug;
const CString CProcessInterface::CMD_ECHO_FILE = TEXT("tmp\\result.echo");
CString CProcessInterface::AddProcessPrefix(CString strCmd, int type)
{
	CString strRetCmd;
	
	if (CMD_POWERSHELL & type)
	{
		strRetCmd = TEXT("powershell \"");
	}
	else {
		strRetCmd = TEXT("cmd.exe /c \"");
	}

	strRetCmd += strCmd;
	strRetCmd += TEXT("\"");

	return strRetCmd;
}

CString CProcessInterface::CleanString(CString str)
{
	if (str.GetLength() < 2)
	{
		return str;
	}
	int length = str.GetLength();
	TCHAR ch = 0;
	int sIndex = 0;
	int eIndex = length-1;

	for (int i = 0; i < length; i++)
	{
		ch = str.GetAt(i);
		if ((ch != TEXT('\r')) &&
			(ch != TEXT('\n')) &&
			(ch != TEXT(' ')) &&
			(ch != TEXT('\t')) &&
			(ch != TEXT('\0')))
		{
			sIndex = i;
			break;
		}
	}

	for (int i = length-1; i >= 0; i--)
	{
		ch = str.GetAt(i);
		if ((ch != TEXT('\r')) &&
			(ch != TEXT('\n')) &&
			(ch != TEXT(' ')) &&
			(ch != TEXT('\t')) &&
			(ch != TEXT('\0')))
		{
			eIndex = i;
			break;
		}
	}

	if (eIndex <= sIndex)
	{
		return TEXT("XX");
	}

	str = str.Mid(sIndex, eIndex-sIndex+1);


	return str;
}

BOOL CProcessInterface::CreateCmdWindow(P_PARAM_T para)
{
	TRACE(TEXT("%s"), para->strCmd);
	TRACE(TEXT("\n"));

	SendDebugMessage(para->strCmd);
	if (para->nType & CMD_BAT_SCRIPT)
	{
		ShellExecute(NULL, NULL, para->strCmd, para->strReturn, NULL, SW_SHOW);
	}
	else if (para->nType & CMD_OPEN)
	{
		ShellExecute(NULL, L"Open", para->strCmd, NULL, NULL, SW_SHOW);
	}
	else if (para->nType & CMD_POWERSHELL)
	{
		if (para->nType & CMD_NO_EXIT)
		{
			para->strCmd.Format(L"/noExit %s", para->strCmd);
		}
		ShellExecute(NULL, L"Open", L"powershell", para->strCmd, NULL, SW_SHOW);
	}
	else 
	{
		para->strCmd = AddProcessPrefix(para->strCmd);
		if (para->nType & CMD_NO_EXIT)
		{
			para->strCmd.Replace(L"/c", L"/k");
		}
		ShellExecute(NULL, TEXT("open"), TEXT("cmd.exe"), para->strCmd, NULL, SW_SHOW);
	}

	return TRUE;
}


void CProcessInterface::Reset(CString tmpDir)
{
	CFileFind finder;  
	CString strPath;
	BOOL bWorking = finder.FindFile(TEXT("tmp\\*.*"));  
	while (bWorking)  
	{  
		bWorking = finder.FindNextFile();  
		strPath = finder.GetFilePath();  
		
		if (strPath.Find(CMD_ECHO_FILE) != -1) {
			DeleteFile(strPath);
		}

	}  
	finder.Close();  
	strPath.Format(L"%s\\tmp", tmpDir);
	CreateDirectory(strPath, NULL);
}

BOOL CProcessInterface::CreateMyProcess(P_PARAM_T para)
{
	DWORD begin = ::GetCurrentTime();
	BOOL bRet = _CreateMyProcess(para);
	DWORD end = ::GetCurrentTime();

	//CTimeSpan span = end - begin;
	TRACE("Cmd Elaps: %dms\n", /*span.GetTimeSpan()*/end-begin);

	return bRet;
}
CCriticalSection g_CS;
#define CMD_ECHO_BUFFER_SIZE (CMD_READ_BUFFER_SIZE)
wchar_t g_wszReadBuffer[CMD_ECHO_BUFFER_SIZE];
BOOL CProcessInterface::GetFileContents(CString path, CString &contents)
{
	g_CS.Lock();
	std::wifstream wifs;
	wifs.open(path);
	if (!wifs.is_open()) {
		TRACE(TEXT("read contents from %s failed!"), path);
		g_CS.Unlock();
		return FALSE;
	}

	CString line;
	while (!wifs.eof()) {
		memset(g_wszReadBuffer, 0, sizeof(wchar_t) * CMD_ECHO_BUFFER_SIZE);
		wifs.getline(g_wszReadBuffer, CMD_ECHO_BUFFER_SIZE);
		line = g_wszReadBuffer;
		contents += line;
		contents += TEXT("\n");
	}
	wifs.close();
	g_CS.Unlock();
	return TRUE;
}

BOOL CProcessInterface::_CreateMyProcess(P_PARAM_T para)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	para->strReturn = TEXT("");
	if (!CreatePipe(&hRead,&hWrite,&sa,0)) {
		para->strReturn = TEXT("Create Pipe Failed!");
		return FALSE;
	}

	STARTUPINFO si = {0};
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 

	si.hStdOutput = hWrite;
	si.hStdError = hWrite;

	PROCESS_INFORMATION pi = {0};  
	si.wShowWindow = SW_HIDE;	

	SendDebugMessage(para->strCmd);
	CString strEchoFile;
	para->strCmd = AddProcessPrefix(para->strCmd, para->nType);
	if (para->nType & CMD_READ_BACK_FILE) {
		strEchoFile.Format(TEXT("%s%d"), CMD_ECHO_FILE, CTime::GetCurrentTime());
		para->strCmd.AppendFormat(TEXT(" > %s"), strEchoFile); 

		//si.dwXCountChars = 1024;
		//si.dwYCountChars = 768;
		//si.dwXSize = 1440;
		//si.dwYSize = 1080;
		//si.dwFlags = STARTF_USECOUNTCHARS | STARTF_USESHOWWINDOW | STARTF_USESIZE;
	}
	TRACE(TEXT("%s"), para->strCmd);
	TRACE(TEXT("\n"));

	if (!CreateProcess(NULL, para->strCmd.GetBuffer(), NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))  
	{  
		para->strReturn = TEXT("Create Process Failed!");
		return FALSE;
	}

	CloseHandle(pi.hThread);	


	if (para->nType & CMD_ABSOLUTE_INFINITE)
	{
		WaitForSingleObject(pi.hProcess,INFINITE);
	}
	else if (para->nType & CMD_JUST_RETURN)
	{
		CloseHandle(hRead);
		CloseHandle(hWrite);
		CloseHandle(pi.hProcess);

		char buffer[257] = {0};
		DWORD dwBytesReadCount = 0;
		DWORD dwBytesRead;
		while (ReadFile(hRead, buffer, 256, &dwBytesRead, NULL))
		{
		}
		return TRUE;
	}
	else if (para->nType & CMD_INFINITE) {
		WaitForSingleObject(pi.hProcess, 15000);
	}
	else 
	{
		WaitForSingleObject(pi.hProcess, para->nWaitMs);
	}

	DWORD dwExitCode;
	GetExitCodeProcess(pi.hProcess,
		&dwExitCode);

	CloseHandle(pi.hProcess);
	CloseHandle(hWrite);

	if (para->nType & CMD_READ_BACK_FILE) 
	{
		GetFileContents(strEchoFile, para->strReturn);
		DeleteFileW(strEchoFile);
		TRACE(TEXT("%s"), para->strReturn);
		TRACE(TEXT("\n"));
		SendDebugMessage(para->strReturn);
		return TRUE;
	}

	//char *buffer = new char[CMD_READ_BUFFER_SIZE];
	//memset(buffer, 0, CMD_READ_BUFFER_SIZE);
	char buffer[CMD_READ_BUFFER_SIZE] = { 0 };

	DWORD dwBytesReadCount = 0;
	DWORD dwBytesRead;
	while (ReadFile(hRead, buffer, CMD_READ_BUFFER_SIZE, &dwBytesRead, NULL))
	{
		para->strReturn += buffer;
		memset(buffer, 0, CMD_READ_BUFFER_SIZE);

		if ((para->nType & CMD_OUTPUT_UPDATE_REALTIME)) {
			SendDebugMessageT(para->strReturn, 1);
		}
		if (para->nType & CMD_READ_BYTES_COUNT) 
		{
			dwBytesReadCount += dwBytesRead;
			
			if (dwBytesReadCount >= para->nReadBytesCount) {
				break;
			}
		}
	}
	//delete buffer;

	CloseHandle(hRead);
	TRACE(TEXT("%s"), para->strReturn);
	TRACE(TEXT("\n"));

	if (!(para->nType & (CMD_NO_OUTPUT_RETURN | CMD_OUTPUT_UPDATE_REALTIME)))
	{
		SendDebugMessageT(para->strReturn, 1);
	}

	return TRUE;
}
