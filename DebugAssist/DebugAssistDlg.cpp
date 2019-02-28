
// DebugAssistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DebugAssist.h"
#include "DebugAssistDlg.h"
#include "afxdialogex.h"
#include "ProcInterface.h"
#include <atlbase.h>
#include <DBT.h>
#include <algorithm>
#include "CSystemInfo.h"
#include "CDialogSelectDriverDir.h"
#include "CDialogDataTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND g_hwndDebug;
CString g_strToolDate;
CString g_strCopyRight;
CDialogDataTool *g_pDlgDataTool;
// CAboutDlg dialog used for App About

#define ST_TIME_PARA(st)   st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond
#define THREAD_START()  TRACE("%s(%d) start\n", __func__, GetCurrentThreadId())
#define THREAD_EXIT()  TRACE("%s(%d) exit\n", __func__, GetCurrentThreadId())

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CAboutDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);

    SetDlgItemText(IDC_STATIC_BUILD_TIME, g_strToolDate);
    SetDlgItemText(IDC_STATIC_COPYRIGHT, g_strCopyRight);

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDebugAssistDlg dialog
void GetCompileDateTime()
{
    const char *szEnglishMonth[12] = {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec",
    };
    char szTmpDate[100] = { 0 };
    char szTmpTime[100] = { 0 };
    char szMonth[10] = { 0 };
    wchar_t szDateTime[250] = { 0 };
    int iYear, iMonth, iDay;
    int iHour, iMin, iSec;

    //获取编译日期、时间 
    sprintf_s(szTmpDate, "%s", __DATE__); //"Sep 18 2010" 
    sprintf_s(szTmpTime, "%s", __TIME__); //"10:59:19" 

    memcpy(szMonth, szTmpDate, 3);
    for (int i = 0; i < 12; i++)
    {
        if (strncmp(szMonth, szEnglishMonth[i], 3) == 0)
        {
            iMonth = i + 1;
            break;
        }
    }
    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpDate + 4, 2);
    iDay = atoi(szMonth);

    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpDate + 7, 4);
    iYear = atoi(szMonth);


    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpTime, 2);
    iHour = atoi(szMonth);

    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpTime + 3, 2);
    iMin = atoi(szMonth);


    memset(szMonth, 0, 10);
    memcpy(szMonth, szTmpTime + 6, 2);
    iSec = atoi(szMonth);


    wsprintf(szDateTime, TEXT("DebugAssist Build@%4d-%02d-%02d_%02d:%02d:%02d"),
        iYear, iMonth, iDay, iHour, iMin, iSec);

    TRACE(TEXT("%s"), szDateTime);
    TRACE("\n");
    g_strToolDate = szDateTime;


    sprintf_s(szTmpDate, "Copyright(C) %d", iYear);
    g_strCopyRight = szTmpDate;

    //g_strToolDate = TEXT("AdbTool, V1.0");
}

#define DestroyToolDialog(pDlg) \
	do {\
		if (pDlg) { \
			pDlg->DestroyWindow();\
			pDlg = NULL;\
		}\
	} while(0)

CDebugAssistDlg::CDebugAssistDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEBUGASSIST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    GetCompileDateTime();
}

void CDebugAssistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COMS, m_cbComPorts);
	DDX_Control(pDX, IDC_RICHEDIT_LOG, m_reLog);
	DDX_Control(pDX, IDC_COMBO_FFU_PATHS, m_cbFfuPaths);
	DDX_Control(pDX, IDC_COMBO_DISKS, m_cbDisks);
	DDX_Control(pDX, IDC_COMBO_WORKSPACE, m_cbWorkspaces);
	DDX_Control(pDX, IDC_COMBO_SOURCE_SYS_FILE, m_cbDriverSourceFile);
	DDX_Control(pDX, IDC_COMBO_DEST_DIR, m_cbDestinationDir);
	DDX_Control(pDX, IDC_COMBO_EXTRA_PARAMS, m_cbExtraParams);
	DDX_Control(pDX, IDC_COMBO_DEBUGGEE_IP, m_cbDebuggeeIPs);
	DDX_Control(pDX, IDC_COMBO_DEBUGGEE_PORT, m_cbDebuggeePorts);
	DDX_Control(pDX, IDC_BTN_FLASH, m_btnFlash);
	DDX_Control(pDX, IDC_EDIT_DST_FILENAME, m_editDstFilename);
	DDX_Control(pDX, IDC_COMBO_FWFITM_PATH, m_cbFwFitMergedPaths);
}

CDebugAssistDlg::~CDebugAssistDlg()
{
    DestroyToolDialog(g_pDlgDataTool);
}

BEGIN_MESSAGE_MAP(CDebugAssistDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_BN_CLICKED(IDC_BTN_COM_DBG, &CDebugAssistDlg::OnBnClickedBtnComDbg)
	ON_MESSAGE(MSG_APPEND_ADB_CMD, &CDebugAssistDlg::OnMsgAppendAdbCmd)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CDebugAssistDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BTN_FLASH, &CDebugAssistDlg::OnBnClickedBtnFlash)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_BCD_EN_DBG, &CDebugAssistDlg::OnBnClickedButtonBcdEnDbg)
	ON_BN_CLICKED(IDC_BUTTON_BCD_DISABLE_DBG, &CDebugAssistDlg::OnBnClickedButtonBcdDisableDbg)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_SYS, &CDebugAssistDlg::OnBnClickedButtonBrowseSys)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_DEST_DIR, &CDebugAssistDlg::OnBnClickedButtonBrowseDestDir)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE, &CDebugAssistDlg::OnBnClickedButtonReplace)
	ON_BN_CLICKED(IDC_BUTTON_EJECT_DRIVE, &CDebugAssistDlg::OnBnClickedButtonEjectDrive)
	ON_BN_CLICKED(IDC_BUTTON_TEST_IP, &CDebugAssistDlg::OnBnClickedButtonTestIp)
	ON_BN_CLICKED(IDC_BUTTON_BACKSTAGE_ADMIN, &CDebugAssistDlg::OnBnClickedButtonBackstageAdmin)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CDebugAssistDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_UEFI_DIR, &CDebugAssistDlg::OnBnClickedButtonUefiDir)
	ON_MESSAGE(UMSG_COMBO_SEL_CHANGE, &CDebugAssistDlg::OnUmsgComSelChange)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_FWFITMERGED, &CDebugAssistDlg::OnBnClickedButtonBrowseFwfitmerged)
	ON_BN_CLICKED(IDC_BUTTON_DD_FLASH, &CDebugAssistDlg::OnBnClickedButtonDdFlash)
	ON_STN_CLICKED(IDC_STATIC_SYSTEM_STATUS, &CDebugAssistDlg::OnStnClickedStaticSystemStatus)
	ON_MESSAGE(UMSG_UPDATE_SYSTEM_STATUS, &CDebugAssistDlg::OnUmsgUpdateSystemStatus)
	ON_WM_TIMER()
    ON_MESSAGE(UMSG_UPDATE_DEST_DRIVER_DIR, &CDebugAssistDlg::OnUmsgUpdateDestDriverDir)
    ON_WM_GETMINMAXINFO()
    ON_BN_CLICKED(IDC_BUTTON_DATA_TOOL, &CDebugAssistDlg::OnBnClickedButtonDataTool)
    ON_MESSAGE(UMSG_COMBO_STRING_DELETE, &CDebugAssistDlg::OnUmsgComboStringDelete)
END_MESSAGE_MAP()



unsigned WINAPI ThreadCloseFormatWnd(LPVOID lP)
{
	static BOOL bThreadRunning = FALSE;
	CDebugAssistDlg *pDlg = (CDebugAssistDlg *)lP;

	THREAD_START();
	if (bThreadRunning) {
		return -1;
	}
	bThreadRunning = TRUE;
	pDlg->CloseFormatWnds();
	bThreadRunning = FALSE;

	THREAD_EXIT();
	return 0;
}

BOOL CDebugAssistDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	switch (nEventType)
	{
	case DBT_DEVICEARRIVAL:
		CleanEnvironment();
		UpdateStatus();
		return TRUE;
	case DBT_DEVICEREMOVECOMPLETE:
		UpdateStatus();
		return TRUE;

	default:
		break;
	}

	return FALSE;
}


void CDebugAssistDlg::RegisterDevice()
{
	const GUID GUID_DEVINTERFACE_LIST[] = {
		{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
		{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
		{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },	/* HID */
		{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } } };

	HDEVNOTIFY   hDevNotify;

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
	{
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(m_hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
		if (!hDevNotify) {
			::MessageBox(m_hWnd, TEXT("×¢²áUSBÉè±¸Í¨ÖªÊ§°Ü"), TEXT("´íÎó"), MB_ICONERROR);
			break;
		}
	}
}

#define MAX_COM_PORT_NUM		(32)
#define COM_PARA_NAME		L"WindbgParameter"
void CDebugAssistDlg::LoadWindbgParameter()
{
	CString strIniPath = m_strSettingIniPath;

	WCHAR strBuffer[MAX_PATH] = { 0 };
	int nRet;

	CString port;
	for (int i = 1; i < MAX_COM_PORT_NUM; i++)
	{
		port.Format(L"COM%d", i);
		nRet = GetPrivateProfileString(COM_PARA_NAME, port, TEXT(""),
			strBuffer, MAX_PATH, strIniPath);
		if (nRet > 0) {
			m_mapWindbgParameter[port] = strBuffer;
		}
	}
	
}

void CDebugAssistDlg::SaveWindbgParameter()
{
	CString strIniPath = m_strSettingIniPath;
	map<CString, CString>::const_iterator cit = m_mapWindbgParameter.begin();

	for (; cit != m_mapWindbgParameter.end(); cit++)
	{
		WritePrivateProfileString(COM_PARA_NAME, cit->first, cit->second, strIniPath);
	}
}


void CDebugAssistDlg::LoadComboStrings(CAutoComboBox & box, LPCWSTR key)
{
	CString strIniPath = m_strSettingIniPath;

	WCHAR strBuffer[MAX_PATH] = { 0 };
	int nRet;

	int count = GetPrivateProfileInt(key, TEXT("count"), 0, strIniPath);
	GetPrivateProfileString(key, TEXT("lastSelected"), TEXT(""),
		strBuffer, MAX_PATH, strIniPath);
	box.SetLastSelected(strBuffer);
	for (int i = 0; i < count; i++)
	{
		CString strNo;
		strNo.Format(TEXT("%d"), i);

		memset(strBuffer, 0, MAX_PATH * sizeof(WCHAR));
		nRet = GetPrivateProfileString(key, strNo, TEXT(""),
			strBuffer, MAX_PATH, strIniPath);
		if (nRet > 0)
		{
			if (wcslen(strBuffer) > 0)
			{
				AddComboString(box, strBuffer);
			}
		}
	}
	SetComboLastSelected(box);
}

void CDebugAssistDlg::LoadComboStrings(set<CString>& setItems, LPCWSTR key)
{
	CString strIniPath = m_strSettingIniPath;

	WCHAR strBuffer[MAX_PATH] = { 0 };
	int nRet;

	int count = GetPrivateProfileInt(key, TEXT("count"), 0, strIniPath);
	GetPrivateProfileString(key, TEXT("lastSelected"), TEXT(""),
		strBuffer, MAX_PATH, strIniPath);

	for (int i = 0; i < count; i++)
	{
		CString strNo;
		strNo.Format(TEXT("%d"), i);

		memset(strBuffer, 0, MAX_PATH * sizeof(WCHAR));
		nRet = GetPrivateProfileString(key, strNo, TEXT(""),
			strBuffer, MAX_PATH, strIniPath);
		if (nRet > 0)
		{
			if (wcslen(strBuffer) > 0)
			{
				setItems.insert(strBuffer);
			}
		}
	}
}


void CDebugAssistDlg::LoadIni()
{
	LoadComboStrings(m_cbFfuPaths, TEXT("FFU"));
	LoadComboStrings(m_cbWorkspaces, TEXT("Workspace"));
	LoadComboStrings(m_cbDriverSourceFile, TEXT("DriverSource"));
	LoadComboStrings(m_setDriverDestDirs, TEXT("DriverDestDir"));
	LoadComboStrings(m_cbExtraParams, TEXT("WindbgExtraParam"));
	LoadComboStrings(m_cbDebuggeeIPs, TEXT("DebuggeeIP"));
	LoadComboStrings(m_cbDebuggeePorts, TEXT("DebuggeePort"));
	LoadComboStrings(m_cbFwFitMergedPaths, TEXT("UbootPath"));
	LoadWindbgParameter();
}

void CDebugAssistDlg::SaveComboStrings(CAutoComboBox &box, LPCWSTR key)
{
	CString strIniPath = m_strSettingIniPath;
	UINT count = box.GetCount();
	CString strCount;
	CString value;

	strCount.Format(TEXT("%d"), count);

	WritePrivateProfileString(key, TEXT("count"), strCount, strIniPath);
	WritePrivateProfileString(key, TEXT("lastSelected"), box.GetLastSelected(), strIniPath);
	for (UINT i = 0; i < count; i++)
	{
		strCount.Format(TEXT("%d"), i);

		box.GetLBText(i, value);

		WritePrivateProfileString(key, strCount, value, strIniPath);
	}
}

void CDebugAssistDlg::SaveComboStrings(set<CString>& setItems, LPCWSTR key)
{
	CString strIniPath = m_strSettingIniPath;
	size_t count = setItems.size();
	CString strCount;
	CString value;

	strCount.Format(TEXT("%d"), count);

	WritePrivateProfileString(key, TEXT("count"), strCount, strIniPath);
	WritePrivateProfileString(key, TEXT("lastSelected"), L"", strIniPath);

	set<CString>::const_iterator cit = setItems.begin();
	for (int i = 0; cit != setItems.end(); cit++,i++)
	{
		strCount.Format(TEXT("%d"), i);

		value = *cit;

		WritePrivateProfileString(key, strCount, value, strIniPath);
	}
}

BOOL CDebugAssistDlg::IsFileDirExist(CString path) const
{
	BOOL exist = FALSE;

	if (_waccess(path, 0) != -1)
	{
		exist = TRUE;
	}

	return exist;
}

CString CDebugAssistDlg::RetriveFilename(CString path)
{
	int nPos = path.ReverseFind(TEXT('\\'));
	CString fileName;
	
	if (nPos != -1) {
		fileName = path.Mid(nPos + 1);
	}

	return fileName;
}

BOOL CDebugAssistDlg::SetComboText(CAutoComboBox & box, CString item)
{
	UINT count = box.GetCount();
	CString value;

	for (UINT i = 0; i < count; i++)
	{
		box.GetLBText(i, value);
		if (!value.Compare(item)) {
			box.SetCurSel(i);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CDebugAssistDlg::SetComboLastSelected(CAutoComboBox & box)
{
	return SetComboText(box, box.GetLastSelected());
}

void CDebugAssistDlg::ShowFileInfo(CAutoComboBox & box) 
{
	CString strFile;
	CString strInfo;
	BOOL bRet;
	SYSTEMTIME st;

	box.GetWindowTextW(strFile);
	if (box == m_cbDestinationDir)
	{
		m_editDstFilename.GetWindowTextW(strInfo);
		strFile.AppendFormat(L"\\%s", strInfo);
	}
	bRet = GetFileTime(strFile, st);
	
	strInfo.Format(L"%s\r\n", strFile);
	if (bRet)
	{
		strInfo.AppendFormat(L"File Time: %04d/%02d/%02d %02d:%02d:%02d", ST_TIME_PARA(st));
	}
	else {
		if (!IsFileDirExist(strFile)) {
			strInfo.AppendFormat(L"File Not Exist!");
		}
		else {
			strInfo.AppendFormat(L"nGet File Time Error!");
		}
		
	}
	AppendDebug(strInfo);
}

void CDebugAssistDlg::SaveIni()
{
	SaveComboStrings(m_cbFfuPaths, TEXT("FFU"));
	SaveComboStrings(m_cbWorkspaces, TEXT("Workspace"));
	SaveComboStrings(m_cbDriverSourceFile, TEXT("DriverSource"));
	SaveComboStrings(m_setDriverDestDirs, TEXT("DriverDestDir"));
	SaveComboStrings(m_cbExtraParams, TEXT("WindbgExtraParam"));
	SaveComboStrings(m_cbDebuggeeIPs, TEXT("DebuggeeIP"));
	SaveComboStrings(m_cbDebuggeePorts, TEXT("DebuggeePort"));
	SaveComboStrings(m_cbFwFitMergedPaths, TEXT("UbootPath"));
	SaveWindbgParameter();
}

unsigned WINAPI ThreadUpdateStatus(LPVOID lP)
{
	static BOOL bThreadRunning = FALSE;
	CDebugAssistDlg *pDlg = (CDebugAssistDlg*)lP;

	THREAD_START();
	if (bThreadRunning) {
		TRACE("ThreadUpdateStatus(%d) exit because already exist\n", GetCurrentThreadId());
		return -1;
	}

	bThreadRunning = TRUE;
	Sleep(500);
	pDlg->EnableCtrls(FALSE);
	pDlg->UpdateStatusProc();
	pDlg->EnableCtrls(TRUE);
	bThreadRunning = FALSE;
	THREAD_EXIT();
	return 0;
}


unsigned WINAPI ThreadUpdateSystemInfo(LPVOID lP)
{
	while (TRUE)
	{
		SYSINFO.GetCpuUsage();
		SYSINFO.GetMemoryUsage();
		SYSINFO.GetNetworkTraffic();

		PostMessage(g_hwndDebug, UMSG_UPDATE_SYSTEM_STATUS, 0, 0);
		Sleep(500);
	}
	return 0;
}

void CDebugAssistDlg::UpdateStatus()
{
	_beginthreadex(NULL, 0, ThreadUpdateStatus, this, 0, NULL);
}

BOOL CDebugAssistDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	g_hwndDebug = this->GetSafeHwnd();
	RegisterDevice();
	GetAppDataConfigDir();
	CProcessInterface::Reset(m_strAppDataConfigDir);
	LoadIni();
	CleanEnvironment();
	UpdateStatusProc();

	_beginthreadex(NULL, 0, ThreadUpdateSystemInfo, this, 0, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDebugAssistDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDebugAssistDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDebugAssistDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDebugAssistDlg::OnBnClickedBtnComDbg()
{

	CString strComPort;
	CString strWorkspace;
	CString strExtraParams;

	m_cbComPorts.GetWindowTextW(strComPort);
	m_cbWorkspaces.GetWindowTextW(strWorkspace);
	m_cbExtraParams.GetWindowTextW(strExtraParams);


	CloseSpecificComWnd(strComPort);

	PARAM_T para;
	para.nType = CMD_JUST_RETURN;
	para.strCmd.Format(TEXT("windbg.exe -a -d -b -v -k com:port=%s,baud=921600 -y \"cache*c:\\symbols; SRV*http://symweb\""), strComPort);

	if (!strWorkspace.IsEmpty()) 
	{
		para.strCmd.AppendFormat(TEXT(" -W %s"), strWorkspace);
	}
	m_mapWindbgParameter[strComPort] = strWorkspace;
	if (!strExtraParams.IsEmpty())
	{
		para.strCmd.AppendFormat(TEXT(" %s"), strExtraParams);
	}

	para.bRet = CProcessInterface::CreateMyProcess(&para);
	

}


void CDebugAssistDlg::GetDiskList()
{
	TCHAR buf[100];
	DWORD len = GetLogicalDriveStrings(sizeof(buf) / sizeof(TCHAR), buf);

	
	m_cbDisks.ResetContent();
	m_vecDisks.clear();
	for (TCHAR* s = buf; *s; s += _tcslen(s) + 1)
	{
		CString strDisks;
		CString sDrivePath(s);
		// 跳过本机系统盘C盘
		if (!sDrivePath.CompareNoCase(TEXT("C:\\")))
		{
			continue;
		}

		m_cbDisks.AddString(sDrivePath);
		m_vecDisks.push_back(DISK_T(s));
	}


	CString driverPath;
	CString drivesInfo;

	m_cbDestinationDir.ResetContent();
	vector<DISK_T>::const_iterator cit = m_vecDisks.begin();
	for (int i = 0; cit != m_vecDisks.end(); cit++, i++)
	{
		drivesInfo.AppendFormat(L"\t[%s](%s)\r\n", cit->root, cit->VolumnLabel);
		if (!cit->VolumnLabel.CompareNoCase(TEXT("EFIESP")))
		{
			SetComboText(m_cbDisks, cit->root);
			m_setDriverDestDirs.insert(cit->root);
		}
		else if (!cit->VolumnLabel.CompareNoCase(TEXT("MainOS")))
		{
			driverPath.Format(L"%s\\Windows\\system32\\drivers", cit->root);
			driverPath.Replace(L"\\\\", L"\\");
			if (IsFileDirExist(driverPath))
			{
				//AddComboString(m_cbDestinationDir, driverPath);
				m_setDriverDestDirs.insert(driverPath);
			}
		}
	}
	drivesInfo.Format(L"Found %d disks:\r\n%s", m_vecDisks.size(), CString(drivesInfo));
	AppendDebug(drivesInfo);

}

void CDebugAssistDlg::GetPhysicalDriveCount()
{
	PARAM_T para;
	CString info;
	para.nType = CMD_WAIT | CMD_NO_OUTPUT_RETURN | CMD_POWERSHELL;
	para.nWaitMs = 1000;
	para.strCmd.Format(TEXT("Get-WmiObject Win32_DiskDrive"));

	CProcessInterface::CreateMyProcess(&para);

	CString strPysclDrive = L"\\\\.\\PHYSICALD";// RIVE"; // 兼容出现乱码的情况
	CString strUpper = para.strReturn.MakeUpper();
	int nDrvPos = strUpper.Find(strPysclDrive);
	m_systemStatus.nDriveCount = 0;
	while (nDrvPos != -1)
	{
		m_systemStatus.nDriveCount++;
		nDrvPos = strUpper.Find(strPysclDrive, nDrvPos + 1);
	}

}
BOOL CDebugAssistDlg::GetDrivePhysicalNo()
{
	PARAM_T para;
	CString info;
	para.nType = CMD_WAIT | CMD_NO_OUTPUT_RETURN;
	para.nWaitMs = 1000;
	para.strCmd.Format(TEXT("wmic diskdrive list"));

	CProcessInterface::CreateMyProcess(&para);
	
	m_nPhysicalDriveNo = -1;

	CString strPysclDrive = L"\\\\.\\PHYSICALD";// RIVE"; // 兼容出现乱码的情况
	CString strUpper = para.strReturn.MakeUpper();

	int nPos = strUpper.Find(TEXT("REMOVABLE MEDIA"));
	int drivePos = strUpper.Find(strPysclDrive, (nPos == -1) ? 0 : nPos);

	if ((nPos == -1) || (drivePos == -1)) {
		TRACE(L"NO Physical Drive NO!");
		return FALSE;
	}
	
	CString strDrvNo;
	WCHAR single;
	drivePos += strPysclDrive.GetLength();
	for (int i = drivePos; i < strUpper.GetLength(); i++)
	{
		single = strUpper.GetAt(i);
		if (::iswdigit(single))
		{
			strDrvNo.AppendChar(single);
		}
		else if ((single == L' ') || (single == L'\t')){
			break;
		}
	}
	m_nPhysicalDriveNo = _wtoi(strDrvNo);
	AppendDebug(strDrvNo);
	TRACE("PhysicalDrive: %d\n", m_nPhysicalDriveNo);

	return !strDrvNo.IsEmpty();
}


DISK_T::DISK_T(CString r) : root(r)
{
	WCHAR label[50] = { 0 };
	WCHAR FileSysName[20] = { 0 };
	::GetVolumeInformation(root, label, 255, &SerialNumber, &MaxCLength, &FileSysFlag, FileSysName, 255);

	VolumnLabel = label;
	FileSys = FileSysName;

}

void CDebugAssistDlg::EnableBcdDebug(BOOL bEnable)
{
	CString strDisk;
	m_cbDisks.GetWindowTextW(strDisk);

	PARAM_T para;
	CString info;
	para.nType = CMD_INFINITE;
	para.strCmd.Format(TEXT("bcdedit.exe /store %sefi\\microsoft\\boot\\bcd /set {default} debug %s "), strDisk,
		bEnable ? TEXT("yes") : TEXT("no"));

	CProcessInterface::CreateMyProcess(&para);

	para.bRet = (para.strReturn.Find(L"complete") != -1);
	info.Format(L"Bcdedit %s Debug %s:\r\n%s", bEnable ? TEXT("Enable") : TEXT("Disable"), para.bRet ? L"Success" : L"FAIL!", para.strReturn);
	MessageBox(info, para.bRet ? L"Info" : L"Error", para.bRet ? MB_ICONINFORMATION : MB_ICONSTOP);
	//para.nType = CMD_BAT_SCRIPT;
	//para.strReturn.Format(TEXT("%s %s"), strDisk, bEnable ? TEXT("yes") : TEXT("no"));
	//para.strCmd.Format(TEXT("bcdedit-enable-debug.bat"));
	//para.bRet = CProcessInterface::CreateCmdWindow(&para);
}

void CDebugAssistDlg::GetAppDataConfigDir()
{
	GetEnvironmentVariable(_T("APPDATA"), m_strAppDataConfigDir.GetBuffer(MAX_PATH + 1), MAX_PATH);
	m_strAppDataConfigDir.ReleaseBuffer();

	m_strAppDataConfigDir.Append(TEXT("\\DebugAssist"));

	if (!PathIsDirectory(m_strAppDataConfigDir)) 
	{
		CreateDirectory(m_strAppDataConfigDir, NULL);
	}
	m_strSettingIniPath.Format(TEXT("%s\\Settings.ini"), m_strAppDataConfigDir);
}

void CDebugAssistDlg::AddFfuPath(CString path)
{
	AddComboString(m_cbFfuPaths, path);
}

void CDebugAssistDlg::AddWorkspace(CString ws)
{
	AddComboString(m_cbWorkspaces, ws);
}

void CDebugAssistDlg::AddSourceFilePath(CString path)
{
	AddComboString(m_cbDriverSourceFile, path);
}

void CDebugAssistDlg::AddComboString(CAutoComboBox & box, CString & item)
{
	if (box.FindString(-1, item) == CB_ERR)
	{
		box.AddString(item);
	}
}

void CDebugAssistDlg::AddComboString(CAutoComboBox & box, LPCWSTR item)
{
	if (box.FindString(-1, item) == CB_ERR)
	{
		box.AddString(item);
	}
}

void CDebugAssistDlg::CleanEnvironment()
{
	_beginthreadex(NULL, 0, ThreadCloseFormatWnd, this, 0, NULL);
}


vector<HWND> GetProcessInfo(CString processName);

void CDebugAssistDlg::CloseSpecificComWnd(CString comPort)
{
	HWND hWnd_Dlg = NULL;
	WCHAR text[MAX_PATH] = { 0 };
	CString titleKey;
	vector<HWND> vecHWND;
	vector<HWND>::const_iterator cit;

	comPort = comPort.MakeUpper();
	titleKey.Format(L"com:port=%s", comPort);
	

	vecHWND = GetProcessInfo(L"windbg.exe");
	cit = vecHWND.begin();

	for (; cit != vecHWND.end(); cit++)
	{
		hWnd_Dlg = *cit;
		::GetWindowText(hWnd_Dlg, text, MAX_PATH);

		CString strTitle(text);
		if (strTitle.Find(titleKey) != -1)
		{
			::PostMessage(hWnd_Dlg, WM_CLOSE, 0, 0);
			Sleep(1000);
			break;
		}
	}
}

void CDebugAssistDlg::CloseFormatWnds()
{
	HWND hWnd_Dlg = NULL;
	// #32770 (Dialog)
	// Caption: Microsoft Windows
	WCHAR text[MAX_PATH] = { 0 };
	int nWndCount = 0;
	int tryCount = 50;

	do {
		hWnd_Dlg = ::FindWindow(L"#32770", L"Microsoft Windows");
		if (!hWnd_Dlg)
		{
			tryCount--;
			Sleep(300);
			continue;
		}

		HWND hwndChild = NULL;

		while (true)
		{
			hwndChild = ::FindWindowEx(hWnd_Dlg, hwndChild, NULL, NULL);

			if (!hwndChild)
			{
				break;
			}
			memset(text, 0, MAX_PATH * sizeof(WCHAR));
			::GetWindowText(hwndChild, text, MAX_PATH);
			if (!StrCmpCW(text, L"格式化磁盘") ||
				!StrCmpCW(text, L"Format disk"))
			{
				::PostMessage(hWnd_Dlg, WM_CLOSE, 0, 0);
				Sleep(100);
				break;
			}
			else if (!StrCmpCW(text, L""))
			{
				nWndCount++;
			}
		}

		if (nWndCount == 1)
		{
			TRACE("Close Format Window: %p\n", hWnd_Dlg);
			::PostMessage(hWnd_Dlg, WM_CLOSE, 0, 0);
			Sleep(500);
		}
		nWndCount = 0;
		
	} while (tryCount);
	
}

void CDebugAssistDlg::ComboboxToVector(CAutoComboBox &box, vector<CString> &vecItems)
{
	CString strItem;
	UINT size = box.GetCount();
	for (UINT i = 0; i < size; i++)
	{
		box.GetLBText(i, strItem);
		vecItems.push_back(strItem);
	}
}
void CDebugAssistDlg::ComboboxToSet(CAutoComboBox &box, set<CString> &setItems)
{
	CString strItem;
	UINT size = box.GetCount();
	for (UINT i = 0; i < size; i++)
	{
		box.GetLBText(i, strItem);
		setItems.insert(strItem);
	}
}

CCriticalSection g_cs;
void CDebugAssistDlg::UpdateStatusProc()
{
	CString port;
	CString lastPort;
	vector<CString> vecComPorts;
	CString lastDir;

	g_cs.Lock();
	m_cbComPorts.GetWindowTextW(lastPort);
	m_cbDestinationDir.GetWindowTextW(lastDir);
	ComboboxToSet(m_cbDestinationDir, m_setDriverDestDirs);
	ComboboxToVector(m_cbComPorts, vecComPorts);

	GetComList();
	GetDiskList();

	GetPhysicalDriveCount();
	if (!GetDrivePhysicalNo())
	{
		m_btnFlash.EnableWindow(FALSE);
	}

	UINT size = m_cbComPorts.GetCount();
	int sel = 0;
	vector<CString>::const_iterator cit;
	for (UINT i = 0; i < size; i++)
	{
		m_cbComPorts.GetLBText(i, port);
		cit = std::find(vecComPorts.begin(), vecComPorts.end(), port);
		if (cit == vecComPorts.end())
		{
			sel = i;
			break;
		}
		else if (size <= vecComPorts.size())
		{
			if (!lastPort.CompareNoCase(port))
			{
				sel = i;
				break;
			}
		}
	}
	if (size > 0)
	{
		m_cbComPorts.SetCurSel(sel);
	}
	UpdateWindbgParameter();
	int dft = UpdateDestDriverFilename();

	m_cbDestinationDir.ResetContent();
	// add valid paths to destination dir
	set<CString>::const_iterator scit = m_setDriverDestDirs.begin();
	for (; scit != m_setDriverDestDirs.end(); scit++)
	{
		if (IsFileDirExist(*scit))
		{
			m_cbDestinationDir.AddString(*scit);
		}
	}
	if (!SetComboText(m_cbDestinationDir, lastDir))
	{
		if (m_cbDestinationDir.GetCount() > 0)
		{
			m_cbDestinationDir.SetCurSel((DFT_UEFI_FIT == dft) ? 0 : 1);
		}
	}

	m_systemStatus.nDiskCount = m_cbDisks.GetCount() + 1;
	m_systemStatus.nPortCount = m_cbComPorts.GetCount();

	//PostMessage(UMSG_UPDATE_SYSTEM_STATUS, 0, 0);
	g_cs.Unlock();
}

void CDebugAssistDlg::EnableCtrls(BOOL bEnable)
{
	static const UINT CONTROLS[] = {
			IDC_BTN_COM_DBG, IDC_BTN_FLASH, IDC_BUTTON_BCD_DISABLE_DBG, IDC_BUTTON_EJECT_DRIVE,
			IDC_BUTTON_BROWSE_DEST_DIR, IDC_BUTTON_BCD_EN_DBG, IDC_BUTTON_BROWSE, IDC_BUTTON_BROWSE_DEST_DIR,
			IDC_BUTTON_BROWSE_SYS, IDC_BUTTON_REPLACE, IDC_COMBO_COMS, IDC_COMBO_DEST_DIR, IDC_COMBO_DISKS,
			IDC_COMBO_EXTRA_PARAMS, IDC_COMBO_FFU_PATHS, IDC_COMBO_SOURCE_SYS_FILE, IDC_COMBO_WORKSPACE,
			IDC_RICHEDIT_LOG, IDC_COMBO_DEBUGGEE_IP, IDC_COMBO_DEBUGGEE_PORT, IDC_EDIT_DST_FILENAME,
			IDC_BUTTON_TEST_IP, IDC_BUTTON_BACKSTAGE_ADMIN, IDC_BUTTON_REFRESH, IDC_BUTTON_UEFI_DIR,
			IDC_BUTTON_BROWSE_FWFITMERGED, IDC_COMBO_FWFITM_PATH, IDC_BUTTON_DD_FLASH,
            IDC_BUTTON_DATA_TOOL
	};

	int size = sizeof(CONTROLS) / sizeof(UINT);
	CWnd *pWnd;
	for (int i = 0; i < size; i++)
	{
		pWnd = GetDlgItem(CONTROLS[i]);
		if (pWnd)
		{
			pWnd->EnableWindow(bEnable);
		}
	}
}

int CDebugAssistDlg::GetComList()
{
	CRegKey RegKey;
	int nCount = 0;
	CString strComInfo;

	m_cbComPorts.ResetContent();
	if (RegKey.Open(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM") == ERROR_SUCCESS)
	{
		while (true)
		{
			TCHAR ValueName[_MAX_PATH];
			BYTE ValueData[_MAX_PATH];
			DWORD nValueSize = _MAX_PATH;
			DWORD nDataSize = _MAX_PATH;
			DWORD nType;
			if (::RegEnumValue(HKEY(RegKey), nCount, ValueName, &nValueSize, NULL, &nType, 
						ValueData, &nDataSize) == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
			m_cbComPorts.AddString(LPCTSTR(ValueData));
			strComInfo.AppendFormat(TEXT("\t[%d]%s\r\n"), nCount, ValueData);
			nCount++;
		}
	}
	strComInfo.Format(TEXT("Found %d COMPorts:\r\n%s"), nCount, CString(strComInfo));
	AppendDebug(strComInfo);
	return nCount;
}

void CDebugAssistDlg::AppendDebug(WPARAM wParam, LPARAM lParam)
{
	CString *pLog = (CString *)wParam;
	CString strNewText;
	CString strTotalText;
	int type = (int)lParam;

	if (type == 0) {
		strNewText = TEXT("\r\n> ");
	}
	else if (type == 1) {
		strNewText = TEXT("\r\n ");
	}
	strNewText += *pLog;
	strNewText += TEXT("\r\n");
	delete pLog;

	TRACE(L"AppendDebug:%d, %s\n", type, strNewText);

	m_reLog.GetWindowTextW(strTotalText);
	strTotalText += strNewText;

	m_reLog.SetWindowTextW(strTotalText);
	m_reLog.PostMessageW(WM_VSCROLL, SB_BOTTOM, 0);

}
void CDebugAssistDlg::AppendDebug(CString text)
{
	PostMessage(MSG_APPEND_ADB_CMD, (WPARAM)(new CString(text)), 0);
}
afx_msg LRESULT CDebugAssistDlg::OnMsgAppendAdbCmd(WPARAM wParam, LPARAM lParam)
{
	AppendDebug(wParam, lParam);
	return 0;
}


void CDebugAssistDlg::OnBnClickedButtonBrowse()
{
	CFileDialog ffuDialog(TRUE, TEXT("Flash file for IOT"),
		NULL, OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		TEXT("FFU|*.ffu|"), this);

	//wchar_t buffer[MAX_PATH] = { 0 };
	//GetModuleFileName(NULL, buffer, MAX_PATH);
	//CString strCurDir = buffer;
	//int pos = strCurDir.ReverseFind(TEXT('\\'));
	//strCurDir = strCurDir.Mid(0, pos);
	//strCurDir += TEXT("\\apks");

	//fileDlgApk.m_ofn.lpstrInitialDir = strCurDir;

	if (ffuDialog.DoModal() != IDOK) {
		return;
	}

	CString strPath = ffuDialog.GetPathName();
	
	AddFfuPath(strPath);
	SetComboText(m_cbFfuPaths, strPath);
}


void CDebugAssistDlg::OnBnClickedBtnFlash()
{
	PARAM_T para;
	CString strPath;

	ShowFileInfo(m_cbFfuPaths);
	m_cbFfuPaths.GetWindowTextW(strPath);
	para.nType = CMD_NO_EXIT;
	para.strCmd.Format(TEXT("dism /apply-image /imagefile:%s /ApplyDrive:\\\\.\\PhysicalDrive%d /SkipPlatformCheck"), 
					strPath, m_nPhysicalDriveNo);
	
	para.bRet = CProcessInterface::CreateCmdWindow(&para);

}


void CDebugAssistDlg::OnClose()
{
	SaveIni();
	CDialogEx::OnClose();
}


void CDebugAssistDlg::OnBnClickedButtonBcdEnDbg()
{
	EnableBcdDebug(TRUE);
}


void CDebugAssistDlg::OnBnClickedButtonBcdDisableDbg()
{
	EnableBcdDebug(FALSE);
}


void CDebugAssistDlg::OnBnClickedButtonBrowseSys()
{
	CFileDialog sysDialog(TRUE, TEXT("Driver Source File"),
		NULL, OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		TEXT("Driver Sys(*.sys)|*.sys|UEFI Firmware|*.fit|Dynamic Link Lib(*.dll)|*.dll|UEFI image(*.fit)|*.fit|Driver Inf(*.inf)|*.inf|All Files|*.*||"), this);


	if (sysDialog.DoModal() != IDOK) {
		return;
	}

	CString strPath = sysDialog.GetPathName();

	AddSourceFilePath(strPath);
	SetComboText(m_cbDriverSourceFile, strPath);
}


void CDebugAssistDlg::OnBnClickedButtonBrowseDestDir()
{

    CDialogSelectDriverDir dlgSelectDir(L"H:\\");
    dlgSelectDir.DoModal();

}

time_t FileTimeToTime_t(const FILETIME &ft)
{
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}

int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2)
{
	FILETIME fTime1 = { 0, 0 };
	FILETIME fTime2 = { 0, 0 };
	SystemTimeToFileTime(&t1, &fTime1);
	SystemTimeToFileTime(&t2, &fTime2);

	time_t tt1 = FileTimeToTime_t(fTime1);
	time_t tt2 = FileTimeToTime_t(fTime2);

	return (int)(tt2 - tt1);
}


BOOL CDebugAssistDlg::GetFileTime(CString path, SYSTEMTIME & st) const
{
	WIN32_FILE_ATTRIBUTE_DATA lpinf;
	SYSTEMTIME stUTC;
	BOOL bRet = GetFileAttributesEx(path, GetFileExInfoStandard, &lpinf);//获取文件信息，path为文件路径
	if (bRet)
	{
		FileTimeToSystemTime(&lpinf.ftLastWriteTime, &stUTC);//转换时间格式：FILETIME到SYSTEMTIME
		SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &st);
	}
		
	return bRet;
}


int CDebugAssistDlg::ToDriveFileType(CString & strFilename) const
{
	CString strFile = strFilename.MakeLower();
	if (strFile.Find(L".sys") != -1)
	{
		return DFT_DRIVER_SYS;
	}
	else if (strFile.Find(L".fit") != -1)
	{
		return DFT_UEFI_FIT;
	}
	else if (strFile.Find(L".inf") != -1)
	{
		return DFT_INF;
	}
	else if (strFile.Find(L".dll") != -1)
	{
		return DFT_DLL;
	}

	return DFT_UNKNOWN;
}



int CDebugAssistDlg::UpdateDestDriverFilename()
{
	CString srcPath;
	CString srcFilename;
	m_cbDriverSourceFile.GetWindowTextW(srcPath);
	srcFilename = RetriveFilename(srcPath);
	m_editDstFilename.SetWindowTextW(srcFilename);

	return ToDriveFileType(srcFilename);
}

void CDebugAssistDlg::OnBnClickedButtonReplace()
{
	CString srcPath;
	CString destPath;
	CString backupPath;
	CString srcFilename;
	CString destFilename;
	SYSTEMTIME stSrc, stDest;
	CString debugInfo;
	CString info;
	BOOL bDestFileExist = FALSE;
	BOOL bRet;

	m_cbDriverSourceFile.GetWindowTextW(srcPath);
	m_cbDestinationDir.GetWindowTextW(destPath);
	srcFilename = RetriveFilename(srcPath);
	m_editDstFilename.GetWindowTextW(destFilename);
	
	if (destFilename.IsEmpty())
	{
		destFilename = srcFilename;
	}

	destPath.AppendFormat(TEXT("\\%s"), destFilename);
	backupPath.Format(L"%s\\%s", m_strAppDataConfigDir, destFilename);

	if (srcPath.IsEmpty() || destPath.IsEmpty())
	{
		MessageBox(TEXT("Empty File Path(s)!"), TEXT("Error"), MB_ICONSTOP);
		return;
	}
	if (!IsFileDirExist(srcPath))
	{
		MessageBox(TEXT("Source File Not Exist!"), TEXT("Error"), MB_ICONSTOP);
		return;
	}

	debugInfo.Format(L"Copy File: \r\nSource: %s\r\nDestination: %s\r\n", srcPath, destPath);
	AppendDebug(debugInfo);

	GetFileTime(srcPath, stSrc);
	bDestFileExist = GetFileTime(destPath, stDest);
	debugInfo.Format(L"%s:\r\nSource File time: %04d/%02d/%02d %02d:%02d:%02d\r\n", srcFilename, ST_TIME_PARA(stSrc));
	if (bDestFileExist) 
	{
		debugInfo.AppendFormat(L"Dest File time: %04d/%02d/%02d %02d:%02d:%02d\r\n", ST_TIME_PARA(stDest));
	}
	AppendDebug(debugInfo);

	// backup first 
	if (bDestFileExist) {
		CString backupInfo;
		bRet = CopyFile(destPath, backupPath, FALSE);
		backupInfo.Format(L"File backup to %s %s!", backupPath, bRet ? L"Success" : L"FAILED");
		AppendDebug(backupInfo);
	}	
	bRet = CopyFile(srcPath, destPath, FALSE);

	info.Format(L"Copy File to %s %s!\r\n\r\n%s", destPath, bRet ? L"Success" : L"FAILED", debugInfo);

	MessageBox(info, bRet ? TEXT("Info") : L"Error", bRet ? MB_ICONINFORMATION : MB_ICONSTOP);
}
#include   <setupapi.h>   
//   需加入setupapi.lib   

#include   <regstr.h>   
#include   <winbase.h>   

#include   <cfgmgr32.h>   
//   需要加入cfgmgr32.lib   

#include   <initguid.h>   
//#include   <usbiodef.h>   
#define READ_BUFFER_SIZE	256
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
	0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
#define   GUID_CLASS_USB_DEVICE                       GUID_DEVINTERFACE_USB_DEVICE   

#define YESORNO(b) (b ? L"Y" : L"N")

inline BOOL IsDisableable(ULONG status)
{
	return !!(DN_DISABLEABLE & status);
}
inline BOOL IsRemoveable(ULONG status)
{
	return !!(DN_REMOVABLE & status);
}

inline BOOL IsUSBDevice(CString &name)
{
	return (name.Find(L"USB") != -1);
}

inline BOOL IsStorageDevice(CString &name)
{
	return ((name.Find(L"存储") != -1) ||
		(name.Find(L"Storage") != -1));
}

int CDebugAssistDlg::RemoveUefiDrive()
{
	HDEVINFO   hDevInfo;

	SP_DEVINFO_DATA   DeviceInfoData;
	DWORD   i;

	//--------------------------------------------------------------------------   
	//   获取设备信息   
	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_CLASS_USB_DEVICE,
		0,   //   Enumerator   
		0,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == INVALID_HANDLE_VALUE) {
		//   查询信息失败   
		TRACE("ERROR   -   SetupDiGetClassDevs()");
		return   1;
	}
	//--------------------------------------------------------------------------   

	//   枚举每个USB设备   
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{
		DWORD DataT;
		WCHAR   hwidBuf[READ_BUFFER_SIZE] = { 0 };
		DWORD   buffersize = READ_BUFFER_SIZE;
		DWORD	retSize = 0;
		//ULONG   len = MAX_PATH;
		CONFIGRET       cr;
		PNP_VETO_TYPE   pnpvietotype;
		//TCHAR   vetoName[MAX_PATH] = { 0 };
		ULONG   ulStatus;
		ULONG   ulProblemNumber;
		WCHAR	friendly_name[READ_BUFFER_SIZE] = { 0 };


		if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, 
					SPDRP_DEVICEDESC, &DataT, (PBYTE)friendly_name, buffersize, &retSize))
		{
			continue;
		}
		// Call function with null to begin with, 
		// then use the returned hwidBuf size 
		// to Alloc the hwidBuf. Keep calling until 
		// success or an unknown failure. 
		// 
		if (!SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&DeviceInfoData,
			SPDRP_HARDWAREID,
			&DataT,
			(PBYTE)hwidBuf,
			buffersize,
			&retSize))
		{
			continue;
		}

		cr = CM_Get_DevNode_Status(&ulStatus,
			&ulProblemNumber,
			DeviceInfoData.DevInst,
			0);
		if (CR_SUCCESS == cr) {
			TRACE("Get Status ok, ret: [%d] status: %x, probno: %x\n", cr, ulStatus, ulProblemNumber);
		}
		else {
			TRACE("Get Status error, ret: [%d], lasterror: %d\n", cr, GetLastError());
			continue;
		}
		TRACE(L"Device[%d]%s(%s), Disableable: %s, Removeable: %s\n", i, friendly_name, hwidBuf, YESORNO(IsDisableable(ulStatus)), YESORNO(IsRemoveable(ulStatus)));

		if (!IsRemoveable(ulStatus) || !IsDisableable(ulStatus)) {
			continue;
		}

		CString strName(friendly_name);
		if (IsUSBDevice(strName) && IsStorageDevice(strName)) {
			CString debugInfo;

			pnpvietotype = PNP_VetoDevice;
			for (int j = 0; j < 3; j++)
			{
				cr = CM_Request_Device_Eject(DeviceInfoData.DevInst,
					&pnpvietotype,
					NULL,
					0,
					0
				);
				if (CR_SUCCESS == cr) {
					break;
				}
				Sleep(500);
			}
		
			if (CR_SUCCESS == cr)
			{
				debugInfo.Format(L"Device: %s(%s) Ejected!\r\n", friendly_name, hwidBuf);
			}
			else {
				debugInfo.Format(L"Eject Device: %s(%s) FAILED!\r\nEject Ret: %d, Last Error: %d\r\n", friendly_name, hwidBuf, cr, GetLastError());
			}
			TRACE(debugInfo);
			AppendDebug(debugInfo);
			MessageBox(debugInfo, (CR_SUCCESS == cr) ? TEXT("Info") : L"Error",
				(CR_SUCCESS == cr) ? MB_ICONINFORMATION : MB_ICONERROR);
		}
	}


	if (GetLastError() != NO_ERROR &&
		GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		//   Insert   error   handling   here.   
		return   1;
	}

	//     Cleanup   
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return  0;
}

unsigned WINAPI ThreadRemoveUefiDrive(LPVOID lP)
{
	CDebugAssistDlg *pDlg = (CDebugAssistDlg *)lP;
	pDlg->EnableCtrls(FALSE);
	pDlg->RemoveUefiDrive();
	pDlg->EnableCtrls(TRUE);

	return 0;
}

void CDebugAssistDlg::OnBnClickedButtonEjectDrive()
{
	_beginthreadex(NULL, 0, ThreadRemoveUefiDrive, this, 0, NULL);
}


void CDebugAssistDlg::OnBnClickedButtonTestIp()
{
	PARAM_T para;
	CString strIp;

	m_cbDebuggeeIPs.GetWindowTextW(strIp);
	para.strCmd.Format(TEXT("ping %s"), strIp);
	para.nType = CMD_WAIT | CMD_OUTPUT_UPDATE_REALTIME;
	para.nWaitMs = 20;

	//para.bRet = CProcessInterface::CreateMyProcess(&para);
	CProcessInterface::CreateCmdWindow(&para);
}


void CDebugAssistDlg::OnBnClickedButtonBackstageAdmin()
{
	PARAM_T para;
	CString strIp;
	CString strPort;

	m_cbDebuggeeIPs.GetWindowTextW(strIp);
	m_cbDebuggeePorts.GetWindowTextW(strPort);

	para.nType = CMD_OPEN;
	para.strCmd.Format(TEXT("http://%s:%s"), strIp, strPort);

    //WinExec((LPCSTR)para.strCmd.GetBuffer(), SW_SHOW);
    //para.strCmd.ReleaseBuffer();
	para.bRet = CProcessInterface::CreateCmdWindow(&para);

}


void CDebugAssistDlg::OnBnClickedButtonRefresh()
{
	UpdateStatus();
	CleanEnvironment();
}


void CDebugAssistDlg::OnBnClickedButtonUefiDir()
{
	CString strIotUefiDisk;
	CString strFile;
	m_cbDisks.GetWindowTextW(strIotUefiDisk);

	if (strIotUefiDisk.IsEmpty()) {
		MessageBox(L"No IOT System Disk", L"Error", MB_ICONERROR);
		return;
	}
	strFile.Format(L"uefi.fit");
	::ShellExecute(NULL, L"Open", strIotUefiDisk, strFile, NULL , SW_SHOW);
}


void CDebugAssistDlg::UpdateWindbgParameter()
{
	CString strCom;
	m_cbComPorts.GetWindowTextW(strCom);

	map<CString, CString>::const_iterator cit = m_mapWindbgParameter.begin();
	for (; cit != m_mapWindbgParameter.end(); cit++)
	{
		if (!cit->first.CompareNoCase(strCom))
		{
			m_cbWorkspaces.SetWindowTextW(cit->second);
			break;
		}

	}
}
afx_msg LRESULT CDebugAssistDlg::OnUmsgComSelChange(WPARAM wParam, LPARAM lParam)
{
	int ctrlID = (int)wParam;

	if (ctrlID == m_cbComPorts.GetDlgCtrlID())
	{
		UpdateWindbgParameter();
	}
	else if (ctrlID == m_cbDriverSourceFile.GetDlgCtrlID())
	{
		int dft = UpdateDestDriverFilename();
		ShowFileInfo(m_cbDriverSourceFile);
		if (DFT_UEFI_FIT == dft)
		{
			if (m_cbDestinationDir.GetCount() > 0)
			{
				m_cbDestinationDir.SetCurSel(0);
			}
		}
	}
	else if ((m_cbFwFitMergedPaths.GetDlgCtrlID() == ctrlID))
	{
		ShowFileInfo(m_cbFwFitMergedPaths);
	}
	else if (m_cbFfuPaths.GetDlgCtrlID() == ctrlID)
	{
		ShowFileInfo(m_cbFfuPaths);
	}
	else if (m_cbDestinationDir.GetDlgCtrlID() == ctrlID)
	{
		ShowFileInfo(m_cbDestinationDir);
	}

	return 0;
}


void CDebugAssistDlg::OnBnClickedButtonBrowseFwfitmerged()
{

    CFileDialog ffuDialog(TRUE, TEXT("Uboot File for IOT"),
        NULL, OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
        TEXT("firmware_fit.merged|*.merged|All|*.*|"), this);

    //wchar_t buffer[MAX_PATH] = { 0 };
    //GetModuleFileName(NULL, buffer, MAX_PATH);
    //CString strCurDir = buffer;
    //int pos = strCurDir.ReverseFind(TEXT('\\'));
    //strCurDir = strCurDir.Mid(0, pos);
    //strCurDir += TEXT("\\apks");

    //fileDlgApk.m_ofn.lpstrInitialDir = strCurDir;

    if (ffuDialog.DoModal() != IDOK) {
        return;
    }

    CString strPath = ffuDialog.GetPathName();

    AddComboString(m_cbFwFitMergedPaths, strPath);
    SetComboText(m_cbFwFitMergedPaths, strPath);
}


void CDebugAssistDlg::OnBnClickedButtonDdFlash()
{
	int nRet;
	nRet = MessageBox(TEXT("Are you sure to dd flash the drive?"), L"Warning", MB_YESNOCANCEL | MB_ICONQUESTION);
	if (nRet != IDYES)
	{
		return;
	}
	ShowFileInfo(m_cbFwFitMergedPaths);

	PARAM_T para;
	CString strPath;

	m_cbFwFitMergedPaths.GetWindowTextW(strPath);
	para.nType = CMD_POWERSHELL | CMD_NO_EXIT;
	para.strCmd.Format(TEXT("dd if=%s of=\\\\.\\PhysicalDrive%d bs=512 seek=2"),
		strPath, m_nPhysicalDriveNo);

	para.bRet = CProcessInterface::CreateCmdWindow(&para);
}



void CDebugAssistDlg::OnStnClickedStaticSystemStatus()
{

}


afx_msg LRESULT CDebugAssistDlg::OnUmsgUpdateSystemStatus(WPARAM wParam, LPARAM lParam)
{
	CString info;
	info.Format(TEXT("[Status] PhysicalDrives: %d COM Ports: %d"), m_systemStatus.nDriveCount,/*
		m_systemStatus.nDiskCount,*/ m_systemStatus.nPortCount);
	info.AppendFormat(L" Up:%s Down:%s", SYSINFO.GetUpTraf(), SYSINFO.GetDownTraf());
	info.AppendFormat(L" CPU:%d%% Memory:%d%%", SYSINFO.m_cpu_usage, SYSINFO.m_memory_usage);

	SetDlgItemText(IDC_STATIC_SYSTEM_STATUS, info);
	return 0;
}


void CDebugAssistDlg::OnTimer(UINT_PTR nIDEvent)
{

	CDialogEx::OnTimer(nIDEvent);
}


afx_msg LRESULT CDebugAssistDlg::OnUmsgUpdateDestDriverDir(WPARAM wParam, LPARAM lParam)
{
    CString *strPath = (CString *)wParam;

    AddComboString(m_cbDestinationDir, *strPath);
    SetComboText(m_cbDestinationDir, *strPath);
    delete strPath;
    return 0;
}


void CDebugAssistDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Add your message handler code here and/or call default

    CDialogEx::OnGetMinMaxInfo(lpMMI);
}



void CDebugAssistDlg::OnBnClickedButtonDataTool()
{
    if ((g_pDlgDataTool != NULL) && g_pDlgDataTool->IsWindowVisible())
    {
        g_pDlgDataTool->ShowWindow(SW_SHOWNORMAL);
    }
    else {
        g_pDlgDataTool = new CDialogDataTool();
        g_pDlgDataTool->Create(IDD_DIALOG_DATA_TOOL, GetDesktopWindow());
        g_pDlgDataTool->ShowWindow(SW_SHOWNORMAL);
    }
}



afx_msg LRESULT CDebugAssistDlg::OnUmsgComboStringDelete(WPARAM wParam, LPARAM lParam)
{
    int ctrlID = (int)lParam;
    if (m_cbDestinationDir.GetDlgCtrlID() == ctrlID)
    {
        CString *pStr = (CString *)wParam;
        m_setDriverDestDirs.erase(*pStr);
        delete pStr;
    }
    return 0;
}


BOOL CDebugAssistDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    static bool m_ctrl_down = false;//此函数第一次调用的时候初始化

    if (pMsg->message == WM_KEYDOWN)
    {

        switch (pMsg->wParam)
        {
            //VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A) 不区分大小写

        case 'E'://Ctrl + A
            if (m_ctrl_down)
            {
                OnBnClickedButtonEjectDrive();
                return TRUE;
            }
            break;
        case VK_CONTROL:
            m_ctrl_down = true;
            break;
        default:
            break;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}
