
// TeeChartDlg.h: 头文件
//

#pragma once
#include "afxwin.h"
#include "mscomm1.h"
#include "tchart2.h"
#include "afxcmn.h"

#include ".\CGridListCtrlEx\CGridListCtrlGroups.h"
#include "CListCtrl_DataModel.h"

#include "CheFileData.h"
#include <string>
#include <map>
using namespace std;
// CTeeChartDlg 对话框
class CTeeChartDlg : public CDialog
{
// 构造
public:
	CTeeChartDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEECHART_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	bool LoadData();
	void UpdateModuleInfo(sCheData*);
	void RefreshListControl();
	void InitListControl();
	afx_msg void OnBnClickedButtonOpen();
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
	CTchart2 m_chart;
	_ConnectionPtr m_pConnection;
	_CommandPtr m_pCommand;
	_RecordsetPtr m_pRecordset;
	CString m_temp;
	CListCtrl m_list;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
protected:
	CGridListCtrlGroups m_ListCtrl;
	CListCtrl_DataModel m_DataModel;
	CImageList m_ImageList;

protected:	
	map<string, CCheFileData*> m_cheFileDataMap;
	CCheFileData*	m_pCurData;

public:
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButtonSelinputfile();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButtonsave();
};
