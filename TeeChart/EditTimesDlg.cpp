// EditTimesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TeeChart.h"
#include "EditTimesDlg.h"
#include "afxdialogex.h"


// CEditTimesDlg dialog

IMPLEMENT_DYNAMIC(CEditTimesDlg, CDialogEx)

CEditTimesDlg::CEditTimesDlg(IChangeTimesNotify* pn, CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditTimesDlg::IDD, pParent)
{
	m_pNotify = pn;
}

CEditTimesDlg::~CEditTimesDlg()
{
}

void CEditTimesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditTimesDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEditTimesDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CEditTimesDlg message handlers


void CEditTimesDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	float fVal = 0;
	CString sText;
	GetDlgItemText(IDC_EDIT1, sText);
	fVal = _ttof(sText);
	if (m_pNotify && m_pNotify->OnChangeTimesNotify(fVal))
	{	
		CDialogEx::OnOK();
	}
}
