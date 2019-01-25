// EditTimesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TeeChart.h"
#include "EditTimesDlg.h"
#include "afxdialogex.h"


// CEditTimesDlg dialog

IMPLEMENT_DYNAMIC(CEditTimesDlg, CBaseDlg)

CEditTimesDlg::CEditTimesDlg(IChangeTimesNotify* pn, CWnd* pParent /*=NULL*/)
	: CBaseDlg(CEditTimesDlg::IDD, pParent)
{
	m_pNotify = pn;
}

CEditTimesDlg::~CEditTimesDlg()
{
}

void CEditTimesDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditTimesDlg, CBaseDlg)
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


BOOL CEditTimesDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	// TODO:  Add extra initialization here
	SetFloatEditId(IDC_EDIT1);

	SetDlgItemInt(IDC_EDIT1, 1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
