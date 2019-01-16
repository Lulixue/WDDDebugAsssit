#include "stdafx.h"
#include "CAutoComboBox.h"


IMPLEMENT_DYNAMIC(CAutoComboBox, CComboBox)

BEGIN_MESSAGE_MAP(CAutoComboBox, CComboBox)
	//消息反射声明
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CAutoComboBox::OnCbnDropdown)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CAutoComboBox::OnCbnSelchange)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, &CAutoComboBox::OnCbnEditupdate)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CAutoComboBox::OnCbnKillfocus)
END_MESSAGE_MAP()

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
	nWitdh += 10;

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
