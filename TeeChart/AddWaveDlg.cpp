// AddWaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TeeChart.h"
#include "AddWaveDlg.h"
#include "afxdialogex.h"


// CAddWaveDlg dialog

IMPLEMENT_DYNAMIC(CAddWaveDlg, CBaseDlg)

CAddWaveDlg::CAddWaveDlg(INewWaveNotify * p, vector<CString>* pw, CWnd* pParent /*=NULL*/)
	: CBaseDlg(CAddWaveDlg::IDD, pParent)
{
	m_pNotify = p;
	m_pWaveNameArr = pw;
}

CAddWaveDlg::~CAddWaveDlg()
{
}

void CAddWaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cb);
}


BEGIN_MESSAGE_MAP(CAddWaveDlg, CBaseDlg)
	ON_BN_CLICKED(IDOK, &CAddWaveDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CAddWaveDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CAddWaveDlg message handlers


void CAddWaveDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if (m_pNotify)
	{
		int nIdx = m_cb.GetCurSel();
		float fTimes;
		float fWidth;
		float fSqrt;
		int nTop;

		CString sText;
		GetDlgItemText(IDC_EDIT_TIME, sText);
		fTimes = _ttof(sText);

		GetDlgItemText(IDC_EDIT_WIDTH, sText);
		fWidth = _ttof(sText);

		GetDlgItemText(IDC_EDIT_SQRT, sText);
		fSqrt = _ttof(sText);

		GetDlgItemText(IDC_EDIT_TOP, sText);
		nTop = _ttoi(sText);

		if(m_pNotify->OnAddNewWave(nIdx, fTimes, fWidth, nTop, fSqrt))
		{
			CDialogEx::OnOK();
		}
	}
}


BOOL CAddWaveDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	// TODO:  Add extra initialization here
	SetFloatEditId(IDC_EDIT_TIME);
	SetFloatEditId(IDC_EDIT_WIDTH);

	if (m_pWaveNameArr)
	{
		for (auto iter = m_pWaveNameArr->begin(); iter != m_pWaveNameArr->end(); iter++)
		{
			m_cb.AddString(*iter);
		}
	}

	if (m_cb.GetCount() > 0)
	{
		m_cb.SetCurSel(0);
		OnCbnSelchangeCombo1();
	}
		
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CAddWaveDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here

	int nSel = m_cb.GetCurSel();
	if (nSel >= 0)
	{
		if (m_pNotify)
		{
			float fTimes;
			float fWidth;
			float fSqrt;
			int nTop;

			if(m_pNotify->GetWaveInfo(nSel, fTimes, fWidth, nTop, fSqrt))
			{
				CString sText;
				sText.Format(L"%.3f", fTimes);
				SetDlgItemText(IDC_EDIT_TIME, sText);

				sText.Format(L"%.3f", fWidth);
				SetDlgItemText(IDC_EDIT_WIDTH, sText);

				sText.Format(L"%d", (int)fSqrt);
				SetDlgItemText(IDC_EDIT_SQRT, sText);

				sText.Format(L"%d", nTop);
				SetDlgItemText(IDC_EDIT_TOP, sText);
			}
		}
	}
}
