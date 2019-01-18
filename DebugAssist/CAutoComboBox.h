#pragma once
#include <afxwin.h>
#include <set>
using std::set;

#define UMSG_COMBO_SEL_CHANGE		(WM_USER + 0x1234)
#define UMSG_LISTBOX_MOUSE_POSITION (WM_USER + 0x300)

#define DEL_BUTTON_MOUSE_ON_COLOR   (RGB(231, 17, 35))      // 红色
#define ITEM_SELECT_TEXT_COLOR      (RGB(255, 255, 255))    // 白色
#define ITEM_UNSELECT_TEXT_COLOR    (RGB(0, 0, 0))          // 黑色
#define ITEM_SELECT_BK_COLOR        (RGB(0, 120, 215))       // 蓝色
#define ITEM_UNSELECT_BK_COLOR      (RGB(255, 255, 255))    // 白色

#define RECT_PARAM(r)   r.top,r.left,r.right, r.bottom

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
    const static UINT BOX_HEIGHT = 18;
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
    BOOL m_bMouseHoverOnDelBtn{ FALSE };//用于判选中项按钮的状态，0正常,1鼠标停留。
    CRect m_rcDelButton;//选中项按钮大小
    static WNDPROC s_oldListWndProc;//用于储存组合框里列表框默认消息处理函数
    static CMap<HWND, HWND, HWND, HWND> s_mapComboWnd;//关联组表框窗口句柄和其下的列表框窗口句柄
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseHover(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
    virtual int CompareItem(LPCOMPAREITEMSTRUCT /*lpCompareItemStruct*/);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    afx_msg LRESULT OnUmsgListboxMousePosition(WPARAM wParam, LPARAM lParam);
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};

