// CDialogDataTool.cpp : implementation file
//

#include "stdafx.h"
#include "DebugAssist.h"
#include "CDialogDataTool.h"
#include "afxdialogex.h"
#include <vector>

// CDialogDataTool dialog

IMPLEMENT_DYNAMIC(CDialogDataTool, CDialogEx)

CDialogDataTool::CDialogDataTool(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DATA_TOOL, pParent)
{

}

CDialogDataTool::~CDialogDataTool()
{
}

void CDialogDataTool::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_DATA_STRING, m_editDataString);
    DDX_Control(pDX, IDC_EDIT_HEX_STRING, m_editHexString);
    DDX_Control(pDX, IDC_EDIT_BINARY_STRING, m_editBinaryString);
    DDX_Control(pDX, IDC_CHECK_LITTLE_ENDIAN, m_chkLittleEndian);
    DDX_Control(pDX, IDC_CHECK_UPPER_CASE, m_chkUpperCase);
    DDX_Control(pDX, IDC_CHECK_HEX_ONLY, m_chkNumberMode);
    DDX_Control(pDX, IDC_EDIT_BINARY_LINE, m_editBinaryLine);
}


BEGIN_MESSAGE_MAP(CDialogDataTool, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_TRANSLATE, &CDialogDataTool::OnBnClickedButtonTranslate)
    ON_BN_CLICKED(IDC_CHECK_UPPER_CASE, &CDialogDataTool::OnBnClickedCheckUpperCase)
    ON_BN_CLICKED(IDC_CHECK_LITTLE_ENDIAN, &CDialogDataTool::OnBnClickedCheckLittleEndian)
    ON_WM_NCDESTROY()
    ON_EN_CHANGE(IDC_EDIT_HEX_STRING, &CDialogDataTool::OnEnChangeEditHexString)
    ON_EN_KILLFOCUS(IDC_EDIT_HEX_STRING, &CDialogDataTool::OnEnKillfocusEditHexString)
    ON_BN_CLICKED(IDC_CHECK_HEX_ONLY, &CDialogDataTool::OnBnClickedCheckHexOnly)
    ON_EN_CHANGE(IDC_EDIT_BINARY_LINE, &CDialogDataTool::OnEnChangeEditBinaryLine)
    ON_EN_CHANGE(IDC_EDIT_DATA_STRING, &CDialogDataTool::OnEnChangeEditDataString)
    ON_BN_CLICKED(IDC_BUTTON_Reset, &CDialogDataTool::OnBnClickedButtonReset)
END_MESSAGE_MAP()


// CDialogDataTool message handlers

LPCWSTR CDialogDataTool::IntToChar(int value)
{
    CString ret;
    switch (value)
    {
    case L'\n':
        return L"\\n";
    case '\r':
        return L"\\r";
    case '\b':
        return L"\\b";
    case '\t':
        return L"\\t";
    default:
        ret.Format(L"%c", value);
        return ret;
    }
}

int CDialogDataTool::CharToInt(CString buf)
{
    if (buf.IsEmpty())
    {
        return -1;
    }
    wchar_t first = buf.GetAt(0);
    if ((first == L'\\') && (buf.GetLength() > 1))
    {
        wchar_t second = buf.GetAt(1);
        switch (second)
        {
        case L'b':
            return L'\b';
        case L'r':
            return L'\r';
        case L'n':
            return L'\n';
        case L't':
            return L'\t';
        default:
            return (int)second;
        }
    }
    else
    {
        return (int)first;
    }
}

BOOL CDialogDataTool::UpdateFromHex()
{
    BOOL bNumberMode = m_chkNumberMode.GetCheck() == BST_CHECKED;
    CString strHex;
    CString strData;
    m_editHexString.GetWindowTextW(strHex);

    CString strChar;
    WCHAR single;
    WCHAR hexBuf[10] = { 0 };
    int length = strHex.GetLength();
    int ret;
    for (int i = 0; i < length; i++)
    {
        single = strHex.GetAt(i);

        if ((single != L',') &&
            (single != L' '))
        {
            strChar.AppendChar(single);
        }

        if ((single == L',') ||
            (single == L' ') || 
            (i == length-1))
        {
            if (strChar.IsEmpty())
            {
                continue;
            }
            memset(hexBuf, 0, sizeof(hexBuf));
            ret = wcstol(strChar, NULL, 16);

            if (bNumberMode)
            {
                strData.AppendFormat(L"%d,", ret);
            }
            else
            {
                strData.AppendFormat(L"%c", IntToChar(ret));
            }

            strChar.Empty();
        }

    }
    m_editDataString.SetWindowTextW(strData);
    return TRUE;
}

BOOL CDialogDataTool::UpdateFromBinaryLine()
{
    CString strBin;
    CString strData;
    m_editBinaryLine.GetWindowTextW(strBin);

    int length = strBin.GetLength();
    int base = 1;
    wchar_t single;
    long long value = 0;
    for (int i = length - 1; i >= 0; i--)
    {
        single = strBin.GetAt(i);

        if (single == L'1')
        {
            value += base;
        }
        else if (single == L' ')
        {
            continue;
        }
        else if (single != L'0')
        {
            MessageBox(L"Invalid Character in binary");
            return FALSE;
        }
        base *= 2;
    }
    strData.Format(L"%d", value);
    m_editDataString.SetWindowTextW(strData);
    return TRUE;
}

CString CDialogDataTool::ToBinLine(long value, BOOL bLittleEndian, BOOL b4Bits)
{
    CString binLine;
    WCHAR binBuf[50] = { 0 };
    errno_t ret = _itow_s(value, binBuf, ARRAYSIZE(binBuf), 2);
    
    if (ret)
    {
        binLine.Format(L"Error(%d)", ret);
        return binLine;
    }

    size_t size = _tcslen(binBuf);
    for (int j = 0; j < (8 - (size % 8)); j++)
    {
        binLine.Append(L"0");
    }
    binLine += binBuf;

    if (bLittleEndian)
    {
        memset(binBuf, 0, 50 * sizeof(WCHAR));

        size = binLine.GetLength();
        for (int k = 0, j = ((int)size - 1); j >= 0; j--, k++)
        {
            binBuf[k] = binLine.GetAt(j);
        }
        binLine.Empty();
        binLine.Format(L"%s", binBuf);
    }

    if (b4Bits)
    {
        CString newBinLine;
        int length = binLine.GetLength();
        int _4times = length / 4;
        int left = length % 4;
        
        for (int i = 0; i < left; i++)
        {
            newBinLine.AppendChar(binLine.GetAt(i));
        }
        if (left > 0)
        {
            newBinLine.AppendChar(L' ');
        }

        for (int i = left, j = 0; i < length; i++)
        {
            newBinLine.AppendChar(binLine.GetAt(i));

            if (((j+1) % 4) == 0)
            {
                newBinLine.AppendChar(L' ');
            }
            j++;
        }
        return newBinLine;

    }

    return binLine;

}

void CDialogDataTool::OnBnClickedButtonTranslate()
{
    // TODO: Add your control notification handler code here

    BOOL bLittleEndian = m_chkLittleEndian.GetCheck() == BST_CHECKED;
    BOOL bUpperCase = m_chkUpperCase.GetCheck() == BST_CHECKED;
    BOOL bNumberMode = m_chkNumberMode.GetCheck() == BST_CHECKED;

    CString strData;
    m_editDataString.GetWindowTextW(strData);


    CString strHex;
    CString strBinary;
    WCHAR single;
    int length = strData.GetLength();

    if (bNumberMode)
    {
        CString strNumber;
        for (int i = 0; i < length; i++)
        {
            single = strData.GetAt(i);
            if (single != L',')
            {
                strNumber.AppendChar(single);
            }
            if ((single == L',') || (i == length - 1))
            {
                long ret = _wtol(strNumber);
                strHex.AppendFormat(bUpperCase ? L"%08X," : L"%08x,", ret);

                strBinary.AppendFormat(bUpperCase ? L"%08lX:\r\n%s\r\n" : L"%08lx:\r\n%s\r\n", ret, ToBinLine(ret, bLittleEndian, TRUE));
                strNumber.Empty();
            }
        }
    }
    else
    {
        CString strChar;
        for (int i = 0, charNum = 0; i < length; i++)
        {
            CString binLine;
            single = strData.GetAt(i);
            strChar.AppendChar(single);
            if (single == L'\\')
            {
                continue;
            }

            int value = CharToInt(strChar);
            if (bUpperCase)
            {
                strHex.AppendFormat(L"%02X,", value);
            }
            else {
                strHex.AppendFormat(L"%02x,", value);
            }

            binLine = ToBinLine(value, bLittleEndian);
            strBinary.AppendFormat(L"%s : ", strChar);
            strChar.Empty();

            if (((charNum + 1) % 2) == 0)
            {
                strBinary.AppendFormat(L"%s\r\n", binLine);
            }
            else {
                strBinary.AppendFormat(L"%s\t", binLine);
            }
            charNum++;
        }
    }
   

    m_editHexString.SetWindowTextW(strHex);
    m_editBinaryString.SetWindowTextW(strBinary);
}


void CDialogDataTool::OnBnClickedCheckUpperCase()
{
    OnBnClickedButtonTranslate();
}


void CDialogDataTool::OnBnClickedCheckLittleEndian()
{
    OnBnClickedButtonTranslate();
}


void CDialogDataTool::OnNcDestroy()
{
    CDialogEx::OnNcDestroy();

    // TODO: Add your message handler code here
    delete this;
}

void CDialogDataTool::OnEnChangeEditHexString()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}


void CDialogDataTool::OnEnKillfocusEditHexString()
{
    // TODO: Add your control notification handler code here
    UpdateFromHex();
    OnBnClickedButtonTranslate();
}


void CDialogDataTool::OnBnClickedCheckHexOnly()
{
    // TODO: Add your control notification handler code here
    UpdateFromHex();
}



void CDialogDataTool::OnEnChangeEditBinaryLine()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    if (UpdateFromBinaryLine())
    {
        OnBnClickedButtonTranslate();
    }
}


void CDialogDataTool::OnEnChangeEditDataString()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}


void CDialogDataTool::OnBnClickedButtonReset()
{
    // TODO: Add your control notification handler code here
    m_editBinaryLine.SetWindowTextW(L"");
    m_editBinaryString.SetWindowTextW(L"");
    m_editDataString.SetWindowTextW(L"");
    m_editHexString.SetWindowTextW(L"");
}
