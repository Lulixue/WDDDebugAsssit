
// DebugAssistDlg.h : header file
//

#pragma once

#include <vector>
#include <set>
#include <map>
#include "CAutoComboBox.h"
using std::vector;
using std::set;
using std::map;


#define UMSG_UPDATE_SYSTEM_STATUS	(WM_USER + 0x2348)

enum DRIVER_FILE_TYPE{
	DFT_UNKNOWN = 0,
	DFT_DRIVER_SYS = 1,
	DFT_UEFI_FIT,
	DFT_DLL,
	DFT_INF,
};

struct DISK_T{

	DISK_T(CString r);

	CString root;
	CString VolumnLabel;
	CString FileSys;
	DWORD SerialNumber;
	DWORD MaxCLength;
	DWORD FileSysFlag;

};


struct SYSTEM_STATUS {
	SYSTEM_STATUS() {
		nShowItem = SYSST_DEFAULT;
		Reset();
	}
	enum {
		SYSST_PORTCNT = 0x01,
		SYSST_DISKCNT = 0x02,
		SYSST_DRVCNT = 0x04,
		SYSST_DEFAULT = SYSST_PORTCNT | SYSST_DISKCNT | SYSST_DRVCNT,
	};


	void Reset() {
		nPortCount = 0;
		nDiskCount = 0;
		nDriveCount = 0;
	}
	int nPortCount;
	int nDiskCount;
	int nDriveCount;

	int nShowItem;
};


// CDebugAssistDlg dialog
class CDebugAssistDlg : public CDialogEx
{
// Construction
public:
	CDebugAssistDlg(CWnd* pParent = nullptr);	// standard constructor

    ~CDebugAssistDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEBUGASSIST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	void CloseFormatWnds();
	BOOL UpdateStatusProc();
	void EnableCtrls(BOOL bEnable);
    void EjectDrive();

// Implementation
private:
    int RemoveUefiDrive();
    BOOL RemoveDisk(CString label, CString &info);
    void UpdateWindbgTypeCombos();
    BOOL GetDbgExeEnable() const {  return (m_chkDebugExe.GetCheck() == BST_CHECKED); }
	void CloseSpecificComWnd(CString strKeyWord, BOOL bComPort = TRUE);
	void GetPhysicalDriveCount();
	BOOL GetFileTime(CString path, SYSTEMTIME &st) const;
	void AppendDebug(WPARAM wParam, LPARAM lParam);
	void AppendDebug(CString text);
	void LoadIni();
	void SaveIni();
	int GetComList();
	BOOL GetDiskList();
	BOOL GetDrivePhysicalNo();
	int ToDriveFileType(CString &strFilename) const;
	int UpdateDestDriverFilename();
	void EnableBcdDebug(BOOL bEnable);
	void GetAppDataConfigDir();
	void AddFfuPath(CString path);
	void AddWorkspace(CString ws);
	void AddSourceFilePath(CString path);
	void AddComboString(CAutoComboBox &box, CString &item);
	void AddComboString(CAutoComboBox & box, LPCWSTR item);
	void CleanEnvironment();
	void LoadWindbgParameter();
	void SaveWindbgParameter();
	void UpdateWindbgParameter();
	void LoadComboStrings(CAutoComboBox &box, LPCWSTR key);
	void LoadComboStrings(set<CString> &setItems, LPCWSTR key);
	void SaveComboStrings(CAutoComboBox &box, LPCWSTR key);
	void SaveComboStrings(set<CString> &setItems, LPCWSTR key);
	BOOL IsFileDirExist(CString path) const;
	CString RetriveFilename(CString path);
	BOOL SetComboText(CAutoComboBox &box, CString item);
	BOOL SetComboLastSelected(CAutoComboBox &box);
	void ShowFileInfo(CAutoComboBox &box);
	void ComboboxToSet(CAutoComboBox &box, set<CString> &setItems);
	void ComboboxToVector(CAutoComboBox &box, vector<CString> &vecItems);
private:
    CString m_strDiskEfi;
	int m_nPhysicalDriveNo;
	CString m_strAppDataConfigDir;
	CString m_strSettingIniPath;
	vector<DISK_T> m_vecDisks;
	vector<CString> m_vecDebugInfoLines;
	map<CString, CString> m_mapWindbgParameter;
	set<CString> m_setDriverDestDirs;
	SYSTEM_STATUS m_systemStatus;
	
protected:
	void UpdateStatus();
	void CDebugAssistDlg::RegisterDevice();
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnComDbg();
	CAutoComboBox m_cbComPorts;
protected:
	afx_msg LRESULT OnMsgAppendAdbCmd(WPARAM wParam, LPARAM lParam);
public:
	CRichEditCtrl m_reLog;
	afx_msg void OnBnClickedButtonBrowse();
	CAutoComboBox m_cbFfuPaths;
	afx_msg void OnBnClickedBtnFlash();
	afx_msg void OnClose();
	CAutoComboBox m_cbDisks;
	afx_msg void OnBnClickedButtonBcdEnDbg();
	afx_msg void OnBnClickedButtonBcdDisableDbg();
	CAutoComboBox m_cbWorkspaces;
	afx_msg void OnBnClickedButtonBrowseSys();
	CAutoComboBox m_cbDriverSourceFile;
	CAutoComboBox m_cbDestinationDir;
	afx_msg void OnBnClickedButtonBrowseDestDir();
	afx_msg void OnBnClickedButtonReplace();
	CAutoComboBox m_cbDbgExecutables;
	afx_msg void OnBnClickedButtonEjectDrive();
	CAutoComboBox m_cbDebuggeeIPs;
	CAutoComboBox m_cbDebuggeePorts;
	afx_msg void OnBnClickedButtonTestIp();
	afx_msg void OnBnClickedButtonBackstageAdmin();
	CButton m_btnFlash;
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonUefiDir();
public:
	afx_msg LRESULT OnUmsgComSelChange(WPARAM wParam, LPARAM lParam);
	CEdit m_editDstFilename;
	CAutoComboBox m_cbFwFitMergedPaths;
	afx_msg void OnBnClickedButtonBrowseFwfitmerged();
	afx_msg void OnBnClickedButtonDdFlash();
	afx_msg void OnStnClickedStaticSystemStatus();
protected:
	afx_msg LRESULT OnUmsgUpdateSystemStatus(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
    afx_msg LRESULT OnUmsgUpdateDestDriverDir(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnBnClickedButtonDataTool();
protected:
    afx_msg LRESULT OnUmsgComboStringDelete(WPARAM wParam, LPARAM lParam);
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedCheckDebugExecutable();
    CButton m_chkDebugExe;
    afx_msg void OnBnClickedBtnPutty();
};
