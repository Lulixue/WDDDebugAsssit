﻿#pragma once


// CDialogDataTool dialog

class CDialogDataTool : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogDataTool)

public:
	CDialogDataTool(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDialogDataTool();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DATA_TOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
    CString IntToChar(int value);
    int CharToInt(CString buf);
    BOOL UpdateFromHex();
    BOOL UpdateFromBinaryLine();
    CString ToBinLine(long value, BOOL bLittleEndian, BOOL b4Bits = FALSE);
public:
    afx_msg void OnBnClickedButtonTranslate();
    CEdit m_editDataString;
    CEdit m_editHexString;
    CEdit m_editBinaryString;
    CButton m_chkLittleEndian;
    CButton m_chkUpperCase;
    afx_msg void OnBnClickedCheckUpperCase();
    afx_msg void OnBnClickedCheckLittleEndian();
    afx_msg void OnNcDestroy();
    afx_msg void OnEnChangeEditHexString();
    afx_msg void OnEnKillfocusEditHexString();
    CButton m_chkNumberMode;
    afx_msg void OnBnClickedCheckHexOnly();
    CEdit m_editBinaryLine;
    afx_msg void OnEnChangeEditBinaryLine();
    afx_msg void OnEnChangeEditDataString();
    afx_msg void OnBnClickedButtonReset();
};
