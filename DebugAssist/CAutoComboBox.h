#pragma once
#include <afxwin.h>
#include <set>
using std::set;

#define UMSG_COMBO_SEL_CHANGE		(WM_USER + 0x1234)
#define UMSG_LISTBOX_MOUSE_POSITION (WM_USER + 0x300)

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
    const static UINT DEL_RECT_WIDTH = 20;
public:
	int SetCurSel(int nSelect);
	CString GetLastSelected() const { return m_strLastSelected; }
	void SetLastSelected(CString item) { m_strLastSelected = item;  }
	afx_msg void OnCbnDropdown();

	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnEditupdate();
	afx_msg void OnCbnKillfocus();
    virtual void PreSubclassWindow();

public:
    BOOL m_bIsDropList{ FALSE };
    BOOL m_MouseHover;//用于判断鼠标当前是否停留在组合框里
    UINT m_TimerID;//计时器ID
    CRect m_comRect;//储存组合框大小s
    CRect m_arrowRect;//组合框右边下拉按钮大小
    BOOL m_arrowHover;//鼠标是否停留在按钮里
    int m_DelButtonState;//用于判选中项按钮的状态，0正常,1鼠标停留。
    CRect m_delRect;//选中项按钮大小
    static WNDPROC m_oldListWndProc;//用于储存组合框里列表框默认消息处理函数
    static CMap<HWND, HWND, HWND, HWND> m_mapWnd;//关联组表框窗口句柄和其下的列表框窗口句柄
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseHover(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
//    afx_msg void OnPaint();
    virtual int CompareItem(LPCOMPAREITEMSTRUCT /*lpCompareItemStruct*/);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    afx_msg LRESULT OnUmsgListboxMousePosition(WPARAM wParam, LPARAM lParam);
};

