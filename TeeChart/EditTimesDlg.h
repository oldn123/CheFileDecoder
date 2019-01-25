#pragma once

#include "BaseDlg.h"

// CEditTimesDlg dialog

class IChangeTimesNotify
{
public:
	virtual bool OnChangeTimesNotify(float) = 0;
};


class CEditTimesDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CEditTimesDlg)

public:
	CEditTimesDlg(IChangeTimesNotify *, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditTimesDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDITTIMES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

protected:
	IChangeTimesNotify * m_pNotify;
public:
	virtual BOOL OnInitDialog();
};
