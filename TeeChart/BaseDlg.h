#pragma once
#include <set>
using namespace std;

// CBaseDlg dialog

class CBaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBaseDlg)

public:
	CBaseDlg(UINT, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBaseDlg();


	void	SetFloatEditId(UINT id){m_floatEditSet.insert(id);}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
	set<UINT>	m_floatEditSet;
};
