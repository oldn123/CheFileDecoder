#pragma once


// CAddWaveDlg dialog

class CAddWaveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddWaveDlg)

public:
	CAddWaveDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddWaveDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDWAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
