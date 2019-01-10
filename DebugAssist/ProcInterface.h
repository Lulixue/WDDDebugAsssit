#pragma once

#include <vector>
#include <set>
#include <map>
using std::map;
using std::set;
using std::vector;

#define MSG_APPEND_ADB_CMD		(WM_USER + 0x4323)


#define CMD_READ_BUFFER_SIZE		256
enum
{
	CMD_INFINITE = 0x01,
	CMD_WAIT = 0x02,
	CMD_JUST_RETURN = 0x04,
	CMD_READ_BYTES_COUNT = 0x08,
	CMD_ABSOLUTE_INFINITE = 0x10,
	CMD_NOADD_SERIALNO_LINE = 0x20,
	CMD_READ_BACK_FILE = 0x40,
	CMD_BAT_SCRIPT = 0x100,
	CMD_OPEN = 0x200,
	CMD_NO_OUTPUT_RETURN = 0x400,
	CMD_OUTPUT_UPDATE_REALTIME = 0x800,
	CMD_POWERSHELL = 0x1000,
	CMD_NO_EXIT = 0x2000,
};

typedef struct 
{
	CString strCmd; 
	CString strReturn;
	int nType;		
	int nWaitMs;				// WaitForObject delay(ms)
	DWORD nReadBytesCount;	// Specify ReadFile Bytes Count
	BOOL bRet;
}PARAM_T, *P_PARAM_T;

class CProcessInterface
{
public:
	static const CString CMD_ECHO_FILE;
	static void Reset(CString tmpDir);
	static BOOL GetFileContents(CString path, CString &contents);
	static CString CleanString(CString str);
	static CString AddProcessPrefix(CString strCmd, int type = 0);
	static BOOL CreateMyProcess(P_PARAM_T para);
	static BOOL _CreateMyProcess(P_PARAM_T para);
	static BOOL CreateCmdWindow(P_PARAM_T para);

};