
// TeeChartDlg.h: 头文件
//

#pragma once
#include "afxwin.h"
#include "mscomm1.h"
#include "tchart2.h"
#include "afxcmn.h"

#include ".\CGridListCtrlEx\CGridListCtrlGroups.h"
#include "CListCtrl_DataModel1.h"
#include "CListCtrl_DataModel2.h"
#include "CListCtrl_DataModel3.h"
#include "CheFileData.h"
#include <string>
#include <map>
#include "afxdtctl.h"
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
	CCheFileData* GetCurrentData(){return m_pCurData;}
	bool LoadData();
	void RefreshChart();
	void UpdateModuleInfo(sCheData*);
	void RefreshListControl(CGridListCtrlGroups*);
	void InitListControl();
	afx_msg void OnBnClickedButtonOpen();
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
	CTchart2 m_chart;
	_ConnectionPtr m_pConnection;
	_CommandPtr m_pCommand;
	_RecordsetPtr m_pRecordset;
	CString m_temp;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
protected:
	CImageList m_ImageList;
	void ResizeTabs();
	void ActiveTab(int);
	void DataToList(CCheFileData * );

protected:	
	map<string, CCheFileData*> m_cheFileDataMap;
	CCheFileData*	m_pCurData;

public:
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButtonSelinputfile();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButtonsave();
	CTabCtrl m_tabControl;

	CGridListCtrlGroups m_list1;
	CGridListCtrlGroups m_list2;
	CGridListCtrlGroups m_list3;

	CListCtrl_DataModel m_DataModel1;
	CListCtrl_DataModel2 m_DataModel2;
	CListCtrl_DataModel3 m_DataModel3;

	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CDateTimeCtrl m_dtDateCtrl;
	CDateTimeCtrl m_dtTimeCtrl;
	afx_msg void OnBnClickedButtonOutdir();
	afx_msg void OnBnClickedButtonReset();
};
