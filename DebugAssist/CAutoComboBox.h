#pragma once
#include <afxwin.h>
#include <set>
using std::set;

#define UMSG_COMBO_SEL_CHANGE		(WM_USER + 0x1234)

class CAutoComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CAutoComboBox)
public:
	CAutoComboBox() {}
	virtual ~CAutoComboBox() {}


protected:
	DECLARE_MESSAGE_MAP()

private:
	CString m_strLastSelected;
public:
	CString GetLastSelected() const { return m_strLastSelected; }
	void SetLastSelected(CString item) { m_strLastSelected = item;  }
	afx_msg void OnCbnDropdown();

	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnEditupdate();
	afx_msg void OnCbnKillfocus();
};

