#pragma once

#include "CAutoComboBox.h"
// CDialogSelectDriverDir dialog
#include <set>
using std::set;
#define UMSG_UPDATE_DEST_DRIVER_DIR     (WM_USER + 0x2345)

class CDialogSelectDriverDir : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSelectDriverDir)

public:
	CDialogSelectDriverDir(CString WindowsDisk, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDialogSelectDriverDir();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SELECT_DRIVER_DEST_DIR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg LRESULT OnUmsgComboSelChange(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    void OwnerToMe();
    void EnableCtrls(BOOL bEnable);
private:
    BOOL GiveEveryoneFullCtrl(CString strDir);
    CString GetDriverDirectory();
    CString GetDriverDirectory(CString key);
    void UpdateSecondDir(CString key);

private:
    CString m_strWindowsDisk;
    set<CString> m_setDriverRepoFiles;
public:
    CAutoComboBox m_cbDriverTopDirs;
    CAutoComboBox m_cbSecondDriverDir;
    CEdit m_editDestDirPath;
    afx_msg void OnBnClickedButtonManual();
    afx_msg void OnBnClickedButtonChangeOwner();
    afx_msg void OnBnClickedButtonPathOk();
};
