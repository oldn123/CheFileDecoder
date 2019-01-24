#pragma once


// CEditTimesDlg dialog

class CEditTimesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditTimesDlg)

public:
	CEditTimesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditTimesDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDITTIMES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
