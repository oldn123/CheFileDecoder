#pragma once
#include "afxwin.h"
#include <vector>

using namespace std;
// CAddWaveDlg dialog

class INewWaveNotify
{
public: 
	virtual bool OnAddNewWave(int nWaveIdx, float fTimes, float fWidth, int nTopVal, float f) = 0;
	virtual bool GetWaveInfo(int nWaveIdx, float & fTimes, float & fWidth, int & nTopVal, float & f) = 0;
};


class CAddWaveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddWaveDlg)

public:
	CAddWaveDlg(INewWaveNotify *, vector<CString>*, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddWaveDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDWAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	INewWaveNotify * m_pNotify;
public:
	afx_msg void OnBnClickedOk();
	CComboBox m_cb;
	vector<CString> * m_pWaveNameArr;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
};
