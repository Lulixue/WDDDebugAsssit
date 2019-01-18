#include "stdafx.h"
#include "CAutoComboBox.h"


IMPLEMENT_DYNAMIC(CAutoComboBox, CComboBox)

BEGIN_MESSAGE_MAP(CAutoComboBox, CComboBox)
	//消息反射声明
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CAutoComboBox::OnCbnDropdown)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CAutoComboBox::OnCbnSelchange)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, &CAutoComboBox::OnCbnEditupdate)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CAutoComboBox::OnCbnKillfocus)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEHOVER()
    ON_WM_MOUSEMOVE()
//    ON_WM_PAINT()
ON_MESSAGE(UMSG_LISTBOX_MOUSE_POSITION, &CAutoComboBox::OnUmsgListboxMousePosition)
END_MESSAGE_MAP()

WNDPROC CAutoComboBox::m_oldListWndProc = NULL;
CMap<HWND, HWND, HWND, HWND> CAutoComboBox::m_mapWnd;
// CAutoComboBox 消息处理程序
void CAutoComboBox::OnCbnDropdown()
{
	CClientDC dc(this);
	int nWitdh = 10;
	int nSaveDC = dc.SaveDC();

	//获取字体信息，
	dc.SelectObject(GetFont());

	//计算最大的显示长度
	for (int i = 0; i < GetCount(); i++)
	{
		CString strLable = _T("");
		GetLBText(i, strLable);

		nWitdh = max(nWitdh, dc.GetTextExtent(strLable).cx);
	}

	//多增加的冗余宽度
	nWitdh += DEL_RECT_WIDTH + 5;

	//设置下拉列表宽度
	SetDroppedWidth(nWitdh);
	//恢复实际dc
	dc.RestoreDC(nSaveDC);
}
int CAutoComboBox::SetCurSel(int nSelect)
{
	int nRet = CComboBox::SetCurSel(nSelect);
	::PostMessage(GetParent()->GetSafeHwnd(), UMSG_COMBO_SEL_CHANGE, (WPARAM)GetDlgCtrlID(), 0);
	return nRet;
}

void CAutoComboBox::OnCbnSelchange()
{
	// TODO: Add your control notification handler code here
	int nSel = GetCurSel();
	if (nSel == -1) {
		m_strLastSelected = TEXT("");
	}
	else {
		GetLBText(nSel, m_strLastSelected);
	}
	::PostMessage(GetParent()->GetSafeHwnd(), UMSG_COMBO_SEL_CHANGE, (WPARAM)GetDlgCtrlID(), 0);
}


void CAutoComboBox::OnCbnEditupdate()
{
	// TODO: Add your control notification handler code here
}


void CAutoComboBox::OnCbnKillfocus()
{
	// TODO: Add your control notification handler code here
	GetWindowText(m_strLastSelected);

	if (!m_strLastSelected.IsEmpty())
	{
		if (FindString(-1, m_strLastSelected) == CB_ERR)
		{
			AddString(m_strLastSelected);
		}
	}
}
LRESULT CALLBACK HookListboxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_MOUSEMOVE)//鼠标移动
    {
        WORD xPos, yPos;
        xPos = LOWORD(lParam);
        yPos = HIWORD(lParam);
        HWND relWnd;
        CAutoComboBox::m_mapWnd.Lookup(hWnd, relWnd);
        ::SendMessage(relWnd, UMSG_LISTBOX_MOUSE_POSITION, xPos, yPos);//发送鼠标位置给列表窗口
    }
    else if (message == WM_LBUTTONDOWN)
    {
        HWND relWnd;
        CAutoComboBox::m_mapWnd.Lookup(hWnd, relWnd);
        //消息类型我是直接以数字发送，觉得看着不舒服的，可自行定义
        //如#define WM_MOUSEPOSITION WM_USER+100
        LRESULT result = ::SendMessage(relWnd, UMSG_LISTBOX_MOUSE_POSITION, (WPARAM)-1, 0);//第三参数-1表明鼠标左键按下
        if (result == 1) 
            return 0;//返回0,不让默认消息处理函数处理WM_LBUTTONDOWN
    }
    //调用列表框之前的默认消息处理函数
    return ::CallWindowProc(CAutoComboBox::m_oldListWndProc, hWnd, message, wParam, lParam);
}

void CAutoComboBox::PreSubclassWindow()
{
    // TODO: Add your specialized code here and/or call the base class

    CComboBox::PreSubclassWindow();

    COMBOBOXINFO cbi;
    cbi.cbSize = sizeof(COMBOBOXINFO);
    ::GetComboBoxInfo(this->m_hWnd, &cbi);//获取组合框信息
        //替换列表框消息处理函数
    m_oldListWndProc = (WNDPROC)::SetWindowLongPtr(cbi.hwndList, GWLP_WNDPROC, reinterpret_cast<LONG_PTR> (HookListboxWndProc));
    LONG_PTR style = GetWindowLongPtr(cbi.hwndCombo, GWL_STYLE);
    if (style & CBS_DROPDOWNLIST)
    {
        m_bIsDropList = TRUE;
    }
    //关于CMap类，今天略微接触了一下，只了解这个类中两个函数的作用
    //不过解决这里的小问题，足够了
    //相互关联，到时可以依据两个中的任一窗口句柄，查询到另一个
    //我想，对于CMap类一定会有解决此问题的方法，但我现在实在不想深入了解 
    //只能给它互相关联，不知道是不是“主流”方法。
    m_mapWnd.SetAt(cbi.hwndList, this->m_hWnd);
    m_mapWnd.SetAt(this->m_hWnd, cbi.hwndList);

    GetClientRect(m_comRect);
    //CString str;
    //str.Format("%d,%d,%d,%d", m_comRect.left, m_comRect.top, m_comRect.right, m_comRect.bottom);
    RECT rect;//编辑框大小
    ::GetClientRect(cbi.hwndItem, &rect);
    //计算按钮大小
    m_arrowRect.left = rect.right + 4;
    m_arrowRect.top = 3;
    m_arrowRect.right = m_comRect.right - 3;
    m_arrowRect.bottom = m_comRect.bottom - 1;
    //设置高度
    //SetItemHeight(-1, 20);
    //SetItemHeight(0, 25);
}


void CAutoComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    CComboBox::OnLButtonDown(nFlags, point);
}


void CAutoComboBox::OnMouseHover(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    m_MouseHover = TRUE;
    TRACE("OnMouseHover: %d\r\n", m_MouseHover);
    CComboBox::OnMouseHover(nFlags, point);
}


void CAutoComboBox::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    CComboBox::OnMouseMove(nFlags, point);
}

#define DEL_BUTTON_MOUSE_ON_COLOR   (RGB(231, 17, 35))      // 红色
#define ITEM_SELECT_TEXT_COLOR      (RGB(255, 255, 255))    // 白色
#define ITEM_UNSELECT_TEXT_COLOR    (RGB(0, 0, 0))          // 黑色
#define ITEM_SELECT_BK_COLOR        (RGB(0, 120, 215))       // 蓝色
#define ITEM_UNSELECT_BK_COLOR      (RGB(255, 255, 255))    // 白色

void CAutoComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);//组合框DC
    CRect itemRect(lpDrawItemStruct->rcItem);//项区域
    int nSate = lpDrawItemStruct->itemState;//项状态
    int nIndex = lpDrawItemStruct->itemID;//项索引
    int nAction = lpDrawItemStruct->itemAction;

    if (nIndex != -1)
    {
        dc.SetBkMode(TRANSPARENT);
        if (nSate & ODS_SELECTED)//如果选中该项
        {
            CPen pen;

            //计算位置
            m_delRect.left = itemRect.right - DEL_RECT_WIDTH;
            m_delRect.top = itemRect.top;
            m_delRect.right = itemRect.right;
            m_delRect.bottom = itemRect.bottom;
            if (nAction != ODA_DRAWENTIRE)
            {
                if (m_DelButtonState == 1) //鼠标停留 
                {
                    dc.FillSolidRect(&m_delRect, DEL_BUTTON_MOUSE_ON_COLOR);
                    pen.CreatePen(1, 2, ITEM_SELECT_TEXT_COLOR);
                    itemRect.right -= DEL_RECT_WIDTH;
                }
                else
                {
                    dc.FillSolidRect(&m_delRect, ITEM_SELECT_BK_COLOR);
                    pen.CreatePen(1, 2, ITEM_SELECT_TEXT_COLOR);
                }
                dc.FillSolidRect(&itemRect, ITEM_SELECT_BK_COLOR);
                // 画X按钮
                m_delRect.top += 5;
                m_delRect.right -= 5;
                m_delRect.bottom -= 5;
                m_delRect.left += 5;
                CRect temDelRect = m_delRect;
                temDelRect.DeflateRect(2, 2, 2, 2);
                dc.SelectObject(&pen);

                dc.MoveTo(temDelRect.left, temDelRect.top);
                dc.LineTo(temDelRect.right, temDelRect.bottom);
                dc.MoveTo(temDelRect.right, temDelRect.top);
                dc.LineTo(temDelRect.left, temDelRect.bottom);
                dc.Draw3dRect(itemRect, RGB(0, 0, 240), RGB(0, 0, 200));//给选中项画边框
            }
            else
            {
                dc.FillSolidRect(&itemRect, ITEM_SELECT_BK_COLOR);
            }
            dc.SetTextColor(ITEM_SELECT_TEXT_COLOR);
        }
        else
        {
            dc.SetTextColor(ITEM_UNSELECT_TEXT_COLOR);
            dc.FillSolidRect(itemRect, ITEM_UNSELECT_BK_COLOR);
        }

        CString str;
        GetLBText(nIndex, str);
        itemRect.DeflateRect(2, 0, 0, 0);

        dc.DrawText(str, itemRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);//显示文本

    }
    
    dc.Detach();
}


//void CAutoComboBox::OnPaint()
//{
//    CPaintDC dc(this); // device context for painting
//                       // TODO: Add your message handler code here
//                       // Do not call CComboBox::OnPaint() for painting messages
//
//    CRect rect;
//    GetClientRect(rect);
//    if (/*m_MouseHover*/TRUE)
//    {
//        dc.Draw3dRect(rect, RGB(102, 199, 250), RGB(99, 98, 255));
//        rect.DeflateRect(1, 1, 1, 1);
//        dc.Draw3dRect(rect, RGB(44, 141, 192), RGB(72, 171, 228));
//        if (/*m_arrowHover*/TRUE)
//        {
//            //按钮边框
//            CRect temArrowRect(m_arrowRect);
//            dc.Draw3dRect(temArrowRect, RGB(90, 140, 167), RGB(72, 172, 228));
//            temArrowRect.DeflateRect(1, 1, 1, 1);
//            dc.Draw3dRect(temArrowRect, RGB(201, 238, 255), RGB(201, 238, 255));
//            temArrowRect.DeflateRect(1, 1, 1, 1);
//            if (this->GetDroppedState())
//                dc.FillSolidRect(temArrowRect, RGB(19, 170, 247));
//            else
//                dc.FillSolidRect(temArrowRect, RGB(135, 205, 241));
//
//        }
//        else  dc.FillSolidRect(m_arrowRect, RGB(255, 255, 255));
//    }
//    else
//    {
//        CRect temRect(rect);
//        dc.Draw3dRect(temRect, RGB(70, 91, 92), RGB(121, 125, 128));//画边框
//        temRect.DeflateRect(1, 1, 1, 1);
//        dc.Draw3dRect(temRect, RGB(255, 255, 255), RGB(255, 255, 255));//画白色边框
//        temRect.DeflateRect(1, 1, 1, 1);
//        dc.Draw3dRect(temRect, RGB(255, 255, 255), RGB(255, 255, 255));//画白色边框
//
//        dc.FillSolidRect(m_arrowRect, RGB(255, 255, 255));
//    }
//    //画按钮里的三角形
//    CPen pen;
//    pen.CreatePen(1, 1, RGB(33, 25, 199));
//    dc.SelectObject(&pen);
//    dc.MoveTo(m_arrowRect.left + 3, m_arrowRect.top + 5);
//    dc.LineTo(m_arrowRect.right - 3, m_arrowRect.top + 5);
//    dc.MoveTo(m_arrowRect.left + 3, m_arrowRect.top + 5);
//    dc.LineTo(m_arrowRect.left + m_arrowRect.Width() / 2, m_arrowRect.bottom - 5);
//    dc.MoveTo(m_arrowRect.right - 3, m_arrowRect.top + 5);
//    dc.LineTo(m_arrowRect.left + m_arrowRect.Width() / 2, m_arrowRect.bottom - 5);
//}


int CAutoComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{

    // TODO:  Add your code to determine the sorting order of the specified items
    // return -1 = item 1 sorts before item 2
    // return 0 = item 1 and item 2 sort the same
    // return 1 = item 1 sorts after item 2

    LPCWSTR pStr1 = (LPCWSTR)lpCompareItemStruct->itemData1;
    LPCWSTR pStr2 = (LPCWSTR)lpCompareItemStruct->itemData2;

    int nResult = StrCmpW(pStr1, pStr2);

    if (nResult < 0) {
        nResult = -1;
    }
    else if (nResult > 0) {
        nResult = 1;
    }

    return nResult;
}


BOOL CAutoComboBox::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    return CComboBox::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CAutoComboBox::OnUmsgListboxMousePosition(WPARAM wParam, LPARAM lParam)
{
    if (m_DelButtonState == 1 && wParam == -1)//如果是单击消息，并且鼠标当前停留在下拉按钮里
    {
        this->DeleteString(GetCurSel());
        this->SelectString(-1, m_strLastSelected);
        Invalidate();
        return 1;
    }
    CPoint point((int)wParam, (int)lParam);
    int result = m_delRect.PtInRect(point);
    if (result && m_DelButtonState == 0)//如果鼠标在组合框内，并且是初次鼠标进入
    {
        m_DelButtonState = 1;
        HWND listWnd;
        m_mapWnd.Lookup(this->m_hWnd, listWnd);
        ::InvalidateRect(listWnd, m_delRect, true);
    }
    else if (result == 0 && m_DelButtonState != 0)
    {
        m_DelButtonState = 0;
        HWND listWnd;
        m_mapWnd.Lookup(this->m_hWnd, listWnd);
        ::InvalidateRect(listWnd, m_delRect, true);
    }
    TRACE("DelButtonState: %d\n", m_DelButtonState);
    TRACE("DelRect: %d, %d, Point: %d\n", m_delRect.left, m_delRect.right, point.x);
    return 0;
}
