// BaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TeeChart.h"
#include "BaseDlg.h"
#include "afxdialogex.h"


// CBaseDlg dialog

IMPLEMENT_DYNAMIC(CBaseDlg, CDialogEx)

CBaseDlg::CBaseDlg(UINT idd, CWnd* pParent /*=NULL*/)
	: CDialogEx(idd, pParent)
{

}

CBaseDlg::~CBaseDlg()
{
}

void CBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseDlg, CDialogEx)
END_MESSAGE_MAP()


// CBaseDlg message handlers
LPCTSTR g_charCode = L"1234567890.";

BOOL CBaseDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	int nNotifyCode = HIWORD(wParam);
	int nCtrlId = LOWORD(wParam);
	if (nNotifyCode == EN_CHANGE)
	{
		if (m_floatEditSet.count(nCtrlId))
		{
			CString sInput;
			GetDlgItemText(nCtrlId, sInput);
			int nLen = sInput.GetLength();
			if (nLen > 0)
			{
				bool bHasFloat = false;
				TCHAR sTmpBuf[250] = {0};
				for (int i = 0; i < nLen; i++)
				{
					CString OneStr = sInput.Mid(i,1); 
					if (OneStr == L".")
					{
						if (!bHasFloat)
						{
							if (i != 0)
							{
								bHasFloat = true;
								_tcscat(sTmpBuf, (LPCTSTR)OneStr);
							}
						}
					}
					else
					{
						if(-1 != OneStr.FindOneOf(g_charCode))
						{
							_tcscat(sTmpBuf, (LPCTSTR)OneStr);
						}
					}	
				}

				if (sInput != sTmpBuf)
				{
					sInput = sTmpBuf;
					SetDlgItemText(nCtrlId, sInput);
					CEdit * pE = (CEdit *)GetDlgItem(nCtrlId);
					pE->SetSel(nLen - 1, nLen - 1, FALSE);
					pE->SetFocus();
				}
			}
		}	
	}
	return CDialogEx::OnCommand(wParam, lParam);
}
