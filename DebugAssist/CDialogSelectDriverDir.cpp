// CDialogSelectDriverDir.cpp : implementation file
//

#include "stdafx.h"
#include "DebugAssist.h"
#include "CDialogSelectDriverDir.h"
#include "afxdialogex.h"
#include "ProcInterface.h"


const CString DRIVERS = L"drivers";
const CString DRIVER_STORE = L"DriverStore";
// CDialogSelectDriverDir dialog
extern HWND g_hwndDebug;

IMPLEMENT_DYNAMIC(CDialogSelectDriverDir, CDialogEx)

CDialogSelectDriverDir::CDialogSelectDriverDir(CString WindowsDisk, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_SELECT_DRIVER_DEST_DIR, pParent)
{
    m_strWindowsDisk = WindowsDisk;
}

CDialogSelectDriverDir::~CDialogSelectDriverDir()
{
}

void CDialogSelectDriverDir::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_DRIVER_SHORTCUTS, m_cbDriverTopDirs);
    DDX_Control(pDX, IDC_COMBO_SECOND_DIR, m_cbSecondDriverDir);
    DDX_Control(pDX, IDC_EDIT_DEST_DIR_PATH, m_editDestDirPath);
}


BEGIN_MESSAGE_MAP(CDialogSelectDriverDir, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_CHANGE_OWNER, &CDialogSelectDriverDir::OnBnClickedButtonChangeOwner)
    ON_MESSAGE(UMSG_COMBO_SEL_CHANGE, &CDialogSelectDriverDir::OnUmsgComboSelChange)
    ON_BN_CLICKED(IDC_BUTTON_MANUAL, &CDialogSelectDriverDir::OnBnClickedButtonManual)
    ON_BN_CLICKED(IDC_BUTTON_PATH_OK, &CDialogSelectDriverDir::OnBnClickedButtonPathOk)
END_MESSAGE_MAP()


// CDialogSelectDriverDir message handlers


BOOL CDialogSelectDriverDir::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_cbDriverTopDirs.AddString(DRIVERS);
    m_cbDriverTopDirs.AddString(DRIVER_STORE);

    m_cbDriverTopDirs.SetCurSel(1);

    CString strDir;
    strDir.Format(L"%s\\FileRepository", GetDriverDirectory(DRIVER_STORE));

    CFileFind finder;
    BOOL isNotEmpty  = finder.FindFile(strDir + _T("\\*.*"));//总文件夹，开始遍历 
    while (isNotEmpty)
    {
        isNotEmpty = finder.FindNextFile();//查找文件 
        CString filename  = finder.GetFileName();//获取文件的路径，可能是文件夹，可能是文件 
        if (finder.IsDirectory() && !finder.IsDots())
        {
            m_setDriverRepoFiles.insert(filename);
        }
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

CString CDialogSelectDriverDir::GetDriverDirectory()
{
    CString topDir;
    m_cbDriverTopDirs.GetWindowTextW(topDir);

    return GetDriverDirectory(topDir);
}

CString CDialogSelectDriverDir::GetDriverDirectory(CString key)
{
    CString retDir;
    if (!key.Compare(DRIVERS))
    {
        retDir.Format(L"%s\\Windows\\system32\\drivers", m_strWindowsDisk);
    }
    else if (!key.Compare(DRIVER_STORE))
    {
        retDir.Format(L"%s\\Windows\\system32\\DriverStore", m_strWindowsDisk);
    }
    retDir.Replace(L"\\\\", L"\\");
    return retDir;
}



BOOL CDialogSelectDriverDir::GiveEveryoneFullCtrl(CString strDir)
{
    CFileFind finder;
    BOOL isNotEmpty = finder.FindFile(strDir + _T("\\*.*"));//总文件夹，开始遍历 

    PARAM_T para;
    para.nType = CMD_POWERSHELL | CMD_INFINITE;

    //para.strCmd.Format(TEXT("icacls %s /T /setowner \"Everyone\""), strDir);
    //para.bRet = CProcessInterface::CreateMyProcess(&para);

    para.strCmd.Format(TEXT("cacls %s\\* /E /C /P \"Everyone\":F"), strDir);
    para.bRet = CProcessInterface::CreateMyProcess(&para);

    while (isNotEmpty)
    {
        isNotEmpty = finder.FindNextFile();//查找文件 
        CString filename = finder.GetFilePath();//获取文件的路径，可能是文件夹，可能是文件 
        if (finder.IsDirectory() && !finder.IsDots())
        {
            GiveEveryoneFullCtrl(filename);
        }
    }

    return (para.strReturn.Find(L"processed") != -1);
}


void CDialogSelectDriverDir::OwnerToMe()
{
    PARAM_T para;
    CString strPath;

    strPath = GetDriverDirectory();
    para.nType = CMD_POWERSHELL | CMD_NO_EXIT | CMD_INFINITE | CMD_NO_OUTPUT_RETURN;
    para.strCmd.Format(L"takeown /f %s /r", strPath);
    para.bRet = CProcessInterface::CreateMyProcess(&para);

    para.nType &= (~CMD_NO_OUTPUT_RETURN);
    para.strCmd.Format(TEXT("cacls %s /T /E /C /P  \"Everyone\":F"), strPath);
    para.bRet = CProcessInterface::CreateMyProcess(&para);

    if ((para.strReturn.Find(L"processed") != -1))
    {
        MessageBox(L"Change owner with Full Control Succeed!");
    }
}

void CDialogSelectDriverDir::EnableCtrls(BOOL bEnable)
{
    static const UINT CONTROLS[] = {
        IDC_BUTTON_PATH_OK, IDC_BUTTON_CHANGE_OWNER,
        IDC_COMBO_SECOND_DIR, IDC_EDIT_DEST_DIR_PATH, 
        IDC_COMBO_DRIVER_SHORTCUTS, IDC_BUTTON_MANUAL,
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

unsigned WINAPI ThreadChangeOwnerToMe(LPVOID lP)
{
    CDialogSelectDriverDir * pDlg = (CDialogSelectDriverDir *)lP;

    pDlg->EnableCtrls(FALSE);
    pDlg->OwnerToMe();
    pDlg->EnableCtrls(TRUE);

    return 0;
}

void CDialogSelectDriverDir::OnBnClickedButtonChangeOwner()
{
    _beginthreadex(NULL, 0, ThreadChangeOwnerToMe, this, 0, NULL);
}

void CDialogSelectDriverDir::UpdateSecondDir(CString key)
{
    set<CString>::const_iterator cit;
    m_cbSecondDriverDir.ResetContent();

    if (!key.Compare(DRIVER_STORE))
    {
        cit = m_setDriverRepoFiles.begin();
        for (; cit != m_setDriverRepoFiles.end(); cit++)
        {
            m_cbSecondDriverDir.AddString(*cit);
        }
    }
}


afx_msg LRESULT CDialogSelectDriverDir::OnUmsgComboSelChange(WPARAM wParam, LPARAM lParam)
{
    int ctrlID = (int)wParam;

    CString strDirPath;
    CString topDir;
    CString secondDir;
    m_cbDriverTopDirs.GetWindowTextW(topDir);
    m_cbSecondDriverDir.GetWindowTextW(secondDir);

    if (ctrlID == m_cbDriverTopDirs.GetDlgCtrlID())
    {
        if (!topDir.Compare(DRIVER_STORE))
        {
            UpdateSecondDir(topDir);
        }
    }

    strDirPath = GetDriverDirectory(topDir);
    if (!topDir.Compare(DRIVER_STORE))
    {
        strDirPath.AppendFormat(L"\\FileRepository\\%s", secondDir);
    }

    m_editDestDirPath.SetWindowTextW(strDirPath);
    m_editDestDirPath.SetSel(0, -1);
    return 0;
}


void CDialogSelectDriverDir::OnBnClickedButtonManual()
{
    BROWSEINFO bifolder;
    wchar_t FileName[MAX_PATH];
    ZeroMemory(&bifolder, sizeof(BROWSEINFO));
    bifolder.hwndOwner = GetSafeHwnd();				// 拥有者句柄
    bifolder.pszDisplayName = FileName;		// 存放目录路径缓冲区
    bifolder.lpszTitle = TEXT("Select Driver Directory");	// 标题
    bifolder.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX; // 新的样式,带编辑框

    LPITEMIDLIST idl = SHBrowseForFolder(&bifolder);
    if (idl == NULL)
    {
        return;
    }

    SHGetPathFromIDList(idl, FileName);

    m_editDestDirPath.SetWindowTextW(FileName);
}


void CDialogSelectDriverDir::OnBnClickedButtonPathOk()
{

    CString strPath;
    m_editDestDirPath.GetWindowTextW(strPath);

    ::PostMessage(g_hwndDebug, UMSG_UPDATE_DEST_DRIVER_DIR, (WPARAM)(new CString(strPath)), 0);
    PostMessage(WM_CLOSE, 0, 0);
}
