
// TeeChartDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TeeChart.h"
#include "TeeChartDlg.h"
#include "afxdialogex.h"

#include "CTChart.h" 
#include "CAxis.h"
#include "CAxes.h"
#include "CScroll.h" 
#include "CSeries.h"

#include <vector>
using namespace std;
#include ".\CGridListCtrlEx\CGridColumnTraitDateTime.h"
#include ".\CGridListCtrlEx\CGridColumnTraitEdit.h"
#include ".\CGridListCtrlEx\CGridColumnTraitCombo.h"
#include ".\CGridListCtrlEx\CGridColumnTraitHyperLink.h"
#include ".\CGridListCtrlEx\CGridRowTraitXP.h"
#include ".\CGridListCtrlEx\ViewConfigSection.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTeeChartDlg 对话框



CTeeChartDlg::CTeeChartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TEECHART_DIALOG, pParent)
{

	m_pListRecover = 0;
	m_nRowRecover  = 0;
	m_nColRecover  = 0;
	m_strTextRecover = L"";
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCurData = NULL;
	CCheFileData::DoInit();
}

CTeeChartDlg::~CTeeChartDlg()
{
	CCheFileData::DoUninit();
}

void CTeeChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART1, m_chart);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);
	//DDX_Control(pDX, IDC_LIST3, m_list3);
	DDX_Control(pDX, IDC_TAB1, m_tabControl);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_dtDateCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_dtTimeCtrl);
}

BEGIN_MESSAGE_MAP(CTeeChartDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTeeChartDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON1, &CTeeChartDlg::OnBnClickedButton1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CTeeChartDlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON4, &CTeeChartDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON_SELINPUTFILE, &CTeeChartDlg::OnBnClickedButtonSelinputfile)
	//ON_BN_CLICKED(IDC_BUTTON6, &CTeeChartDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTONSAVE, &CTeeChartDlg::OnBnClickedButtonsave)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CTeeChartDlg::OnTcnSelchangeTab1)
	ON_WM_SIZE()
	//ON_BN_CLICKED(IDC_BUTTON_OUTDIR, &CTeeChartDlg::OnBnClickedButtonOutdir)
	//ON_BN_CLICKED(IDC_BUTTON_RESET, &CTeeChartDlg::OnBnClickedButtonReset)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_ADDWAVE, &CTeeChartDlg::OnBnClickedButtonAddwave)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEWAVE, &CTeeChartDlg::OnBnClickedButtonRemovewave)
	ON_BN_CLICKED(IDC_BUTTON_EDITTIMES, &CTeeChartDlg::OnBnClickedButtonEdittimes)
	//ON_BN_CLICKED(IDC_BUTTON_FIX, &CTeeChartDlg::OnBnClickedButtonFix)
END_MESSAGE_MAP()


// CTeeChartDlg 消息处理程序
void CTeeChartDlg::RefreshListControl(CGridListCtrlGroups* plist)
{
	plist->DeleteAllItems();

	IDataModelForList * pData = NULL;
	if(plist == &m_list1)
	{
		pData = &m_DataModel1;
	}
	else if (plist == &m_list2)
	{
		pData = &m_DataModel2;
	}
	else if (plist == &m_list3)
	{
		pData = &m_DataModel3;
	}

	// Insert data into list-control by copying from datamodel
	int nItem = 0;
	for(size_t rowId = 0; rowId < pData->GetRowIds() ; ++rowId)
	{
		nItem = plist->InsertItem(++nItem, pData->GetCellText(rowId,0));
		plist->SetItemData(nItem, rowId);
		for(int col = 0; col < pData->GetColCount() ; ++col)
		{
			int nCellCol = col+1;	// +1 because of hidden column
			const CString& strCellText = pData->GetCellText(rowId, col);
			plist->SetItemText(nItem, nCellCol, strCellText);
		}
		plist->SetCellImage(nItem, 1, nItem); // Assign flag-images
	}
}

void CTeeChartDlg::OnResetSqrt(int nRow)
{
	if (m_pCurData)
	{	
		UpdateModuleInfo(m_pCurData->GetData());
		RefreshListControl(&m_list1);
	}
}

class CUserTraitEdit : public CGridColumnTraitEdit
{
public:
	CUserTraitEdit(CTeeChartDlg* pDlg, int Type){
		m_pDlg = pDlg;
		m_nType = Type;
		m_pLastModiCtrl = NULL;
	}
	virtual CWnd* OnEditBegin(CGridListCtrlEx& owner, int nRow, int nCol)
	{
		m_pLastModiCtrl = &owner;
		m_txtBak = owner.GetItemText(nRow, nCol);
		return __super::OnEditBegin(owner, nRow, nCol);
	}
	 void OnEditEnd(int nRow, int nCol, LPTSTR sText) override{
		if (m_nType == 0)
		{
			if (m_pDlg)
			{
				bool bChangeOk = false;
				switch(nCol)
				{
				case 5:
					{
						bChangeOk = m_pDlg->GetCurrentData()->ChangeWaveTop(nRow, _ttoi(sText));
					}
					break;
				case 2:
					{
						bChangeOk = m_pDlg->GetCurrentData()->ChangeWaveTimePos(nRow, _ttof(sText));
					}
					break;
				case 6:
					{
						bChangeOk = m_pDlg->GetCurrentData()->ChangeWaveTimeWidth(nRow, _ttof(sText));
					}
					break;
				case 3:
					{
						bChangeOk = m_pDlg->GetCurrentData()->ChangeWaveSqrt(nRow, _ttof(sText));
						if (bChangeOk)
						{
							m_pDlg->OnResetSqrt(nRow);
						}
					}
					break;
				default:
					{

					}
				}

				if (bChangeOk)
				{
					m_pDlg->RefreshChart();
				}
				else
				{
					AfxMessageBox(m_pDlg->GetCurrentData()->GetLastErr());
					
					if (m_pLastModiCtrl)
					{
						m_pDlg->RecoverText(m_pLastModiCtrl, nRow, nCol, (LPCTSTR)m_txtBak);
// 						_tcscpy(sText, (LPCTSTR)m_txtBak); 
// 						m_pLastModiCtrl->SetItemText(nRow, nCol, m_txtBak);
// 						m_pLastModiCtrl = NULL;
					}
				}
			
			}
		}
		else if (m_nType == 1)
		{
		}
		else
		{

		}
	 }
protected:
	CGridListCtrlEx * m_pLastModiCtrl;
	CTeeChartDlg* m_pDlg;
	int m_nType;
	CString m_txtBak;
};

void CTeeChartDlg::RecoverText(CGridListCtrlEx * pc, int nRow, int nCol, LPCTSTR strText)
{
	m_pListRecover = pc;
	m_nRowRecover  = nRow;
	m_nColRecover  = nCol;
	m_strTextRecover = strText;
	SetTimer(100, 200, NULL);
}

void CTeeChartDlg::InitListControl()
{
	{
		m_list1.SetCellMargin(1.2);
		CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;
		m_list1.SetDefaultRowTrait(pRowTrait);
		m_list1.EnableVisualStyles(true);

		// Create Columns
		m_list1.InsertHiddenLabelColumn();	// Requires one never uses column 0

		for(int col = 0; col < m_DataModel1.GetColCount() ; ++col)
		{
			bool bCanEdit = false;
			const CString& title = m_DataModel1.GetColTitle(col, bCanEdit);
			CGridColumnTrait* pTrait = NULL;
			if (bCanEdit)	// City
			{
				pTrait = new CUserTraitEdit(this, 0);
			}
			else
			{
				pTrait = new CGridColumnTraitText;
			}

			m_list1.InsertColumnTrait(col+1, title, LVCFMT_CENTER, col == 0 ? 100 : 120, col, pTrait);
		}
	}

	{
		m_list2.SetCellMargin(1.2);
		CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;
		m_list2.SetDefaultRowTrait(pRowTrait);
		m_list2.EnableVisualStyles(true);

		// Create Columns
		m_list2.InsertHiddenLabelColumn();	// Requires one never uses column 0

		for(int col = 0; col < m_DataModel2.GetColCount() ; ++col)
		{
			bool bCanEdit = false;
			const CString& title = m_DataModel2.GetColTitle(col, bCanEdit);
			CGridColumnTrait* pTrait = NULL;
			if (bCanEdit)	// City
			{
				pTrait = new CUserTraitEdit(this, 1);
			}
			else
			{
				pTrait = new CGridColumnTraitText;
			}
			m_list2.InsertColumnTrait(col+1, title, LVCFMT_CENTER, col == 0 ? 100 : 120, col, pTrait);
		}
	}

	if(0)
	{
		m_list3.SetCellMargin(1.2);
		CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;
		m_list3.SetDefaultRowTrait(pRowTrait);
		m_list3.EnableVisualStyles(true);

		// Create Columns
		m_list3.InsertHiddenLabelColumn();	// Requires one never uses column 0

		for(int col = 0; col < m_DataModel3.GetColCount() ; ++col)
		{
			bool bCanEdit = false;
			const CString& title = m_DataModel3.GetColTitle(col, bCanEdit);
			CGridColumnTrait* pTrait = NULL;
			if (bCanEdit)	// City
			{
				pTrait = new CUserTraitEdit(this, 1);
			}
			else
			{
				pTrait = new CGridColumnTraitText;
			}
			m_list3.InsertColumnTrait(col+1, title, LVCFMT_CENTER, 80, col, pTrait);
		}
	}
	

}

void CTeeChartDlg::DataToList(CCheFileData* pDataFile)
{
	m_DataModel3.Clear();

	int nRows = pDataFile->GetDataCnt() / 10;

	for (int i = 0; i < nRows; i++)
	{
		vector<CString> dataRow;
		int nFrom = i * 10;
		for (int j = 0; j < 10; j++)
		{
			double dval = 0;
			if (pDataFile->GetDataByIdx(nFrom + j, dval))
			{
				CString strItem;
				strItem.Format(L"%d", (int)dval);
				dataRow.push_back(strItem);
			}
		}
		CString sRowIdx;
		sRowIdx.Format(L"%d",nFrom);
		m_DataModel3.AddItem(sRowIdx, dataRow);
	}

	RefreshListControl(&m_list3);
}

void CTeeChartDlg::UpdateModuleInfo(sCheData* pData)
{
	m_DataModel1.Clear();
	m_DataModel2.Clear();

	sJfItem3 item3;
	item3.fContentsVal = 0;
	item3.sGroupName = L"";

	for (int i = 0; i < pData->sJfData2.verItems.size(); i++)
	{
		auto iter = pData->sJfData2.verItems[i];
		sJfItem3 * pItem3 = i < pData->sJgData.verItems.size() ? &pData->sJgData.verItems[i] : &item3;

		CString sNumber;
		sNumber.Format(L"%d", /*iter.nIdx*/i + 1);

		CString sliveTime;
		sliveTime.Format(L"%.2f", m_pCurData->GetXValue(iter.nTopDataIdx));

		CString sTopSqrt;
		sTopSqrt.Format(L"%d", iter.nTopSqrt);

		CString sTopSqrtPer;
		sTopSqrtPer.Format(L"%.2f%%", iter.nTopSqrt * 100 / pData->fTopSqrtTotal);

		CString sTopHVal;
		sTopHVal.Format(L"%d", iter.nTopHVal);
	
		CString sTopWVal;
		sTopWVal.Format(L"%.3f", m_pCurData->GetXValue(iter.nEndDataIdx) - m_pCurData->GetXValue(iter.nBeginDataIdx) );

		CString sTopStype;
		sTopStype.Format(L"%s", "");

		m_DataModel1.AddItem(sNumber, sliveTime, sTopSqrt, sTopSqrtPer, sTopHVal, sTopWVal, sTopStype );
	}

	for (int i = 0; i < pData->sJgData.verItems.size(); i++)
	{
		sJfItem3 * pItem3 = &pData->sJgData.verItems[i];

		CString sNumber;
		sNumber.Format(L"%d", pItem3->nIdx + 1);

		CString sliveTime;
		sliveTime.Format(L"%.2f", pItem3->fLiveTime);

		CString sTopSqrt;
		sTopSqrt.Format(L"%d", (int)pItem3->fTopSqrt);

		CString sTopHVal;
		sTopHVal.Format(L"%d", (int)pItem3->fTopHVal);

		CString sGroupName;
		sGroupName.Format(L"%s", pItem3->sGroupName);

		CString sContentsVal;
		sContentsVal.Format(L"%.4f", pItem3->fContentsVal);

		CString sTopStype;
		sTopStype.Format(L"%d", 0);

		m_DataModel2.AddItem(sNumber, sGroupName, sliveTime, sTopSqrt, sTopHVal, sContentsVal, sTopStype );
	}
}

CString GetPath(LPCTSTR lpszName)
{
	CString strPath(lpszName);
	int nPos1 = strPath.ReverseFind(_T('\\'));		//先取文件名
	int nPos2 = strPath.ReverseFind(_T('\/'));
	int nPos = nPos2 < nPos1 ? nPos1 : nPos2;
	if (nPos > 0)
		strPath = strPath.Left(nPos);

	return strPath;
}


CString GetFileName(LPCTSTR lpszFullFileName, bool bGetExtName)
{
	CString strFileName(lpszFullFileName);
	CString sName = strFileName;
	int nPos1 = strFileName.ReverseFind(_T('\\'));		//先取文件名
	int nPos2 = strFileName.ReverseFind(_T('\/'));
	int nPos = nPos2 < nPos1 ? nPos1 : nPos2;
	if (nPos > 0)
	{
		sName = strFileName.Right(strFileName.GetLength() - nPos - 1);
	}

	nPos = sName.ReverseFind(_T('.'));

	if (!bGetExtName && nPos > 0)
	{
		CString sNewName = sName.Left(nPos);
		sName = sNewName;
	}

	return sName;
}

void CTeeChartDlg::RebuildUI()
{
	UpdateModuleInfo(m_pCurData->GetData());
	RefreshListControl(&m_list1);
	RefreshListControl(&m_list2);
	COleDateTime dtData = m_pCurData->GetData()->dtOle1;
	m_dtDateCtrl.SetTime(dtData);
	m_dtTimeCtrl.SetTime(dtData);
	RefreshChart();
}

bool CTeeChartDlg::LoadData()
{
	CCheFileData * pData = new CCheFileData;
	CString sInput;
	GetDlgItemText(IDC_EDIT_INPUTPATH, sInput);
	
	if (!pData->LoadFile(sInput))
	{
		delete pData;
		return false;
	}
	USES_CONVERSION;
	string sKey = T2A(sInput);
	
	if (m_cheFileDataMap.count(sKey))
	{
		delete m_cheFileDataMap[sKey];
	}
	m_cheFileDataMap[sKey] = pData;
	m_pCurData = pData;

	CString sFile2;
	GetDlgItemText(IDC_EDIT_OUTPUTPATH, sFile2);
	bool bNeedSetPath = sFile2.GetLength() < 1;
	sFile2 = sInput.Left(sInput.GetLength() - 4) + L".che";//L"_New.che";
	if (bNeedSetPath)
	{	
		SetDlgItemText(IDC_EDIT_OUTPUTPATH, GetPath(sFile2));
	}
	
	SetDlgItemText(IDC_EDIT_NAME, GetFileName(sFile2, true));

	SetDlgItemInt(IDC_EDIT_DATAFROM, 0);
	SetDlgItemInt(IDC_EDIT_DATATO, pData->GetData()->nDataCnt);

	RebuildUI();

	return true;
}


BOOL CTeeChartDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

//	CSeries(m_chart.Series(0)).Add(0, _T("aaaaaaaa"), 1);


	CSeries serDemo = (CSeries)m_chart.Series(0);
 	serDemo.AddNullXY(0.3,7,NULL );	
	serDemo.AddNullXY(0.5,5,L"aaaabbb" );
 	serDemo.AddNullXY(0.8,2,NULL );

	serDemo.put_PointValue(0.3,7 );	
	serDemo.put_PointValue(0.5,5 );
	serDemo.put_PointValue(0.8,2 );

	CAxis aleft = ((CAxes)m_chart.get_Axis()).get_Left();
	CAxis abottom = ((CAxes)m_chart.get_Axis()).get_Bottom();
	aleft.put_Automatic(false);
	aleft.put_Maximum(800);
	aleft.put_Increment(1);
	aleft.put_Minimum(50);
//	aleft.put_StartPosition(60);

	InitListControl();

	int nItem = m_tabControl.InsertItem(0, L"积分结果");
	nItem = m_tabControl.InsertItem(1, L"计算结果");
	//nItem = m_tabControl.InsertItem(2, L"数据编辑");


	m_list1.SetParent(&m_tabControl);
	m_list2.SetParent(&m_tabControl);
	//m_list3.SetParent(&m_tabControl);	


	ActiveTab(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTeeChartDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTeeChartDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTeeChartDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTeeChartDlg::OnBnClickedButtonOpen()
{
	
}


BEGIN_EVENTSINK_MAP(CTeeChartDlg, CDialog)
	ON_EVENT(CTeeChartDlg, IDC_MSCOMM1, 1, CTeeChartDlg::OnCommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()


void CTeeChartDlg::OnCommMscomm1()
{
	
}


void CTeeChartDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	LoadData();
}


void CTeeChartDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CTeeChartDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
}


void CTeeChartDlg::OnBnClickedButtonSelinputfile()
{
	// TODO: Add your control notification handler code here

	CFileDialog filedialog(TRUE,NULL,L"",OFN_HIDEREADONLY, L"All Files(*.che)|*.che||",NULL);
	if(filedialog.DoModal() == IDOK)
	{
		CString sFile = filedialog.GetPathName();
		SetDlgItemText(IDC_EDIT_INPUTPATH, sFile);

		LoadData();
	}
}


void CTeeChartDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
// 	CStringA sTips;
// 	for (auto iter = m_cheFileDataMap.begin(); iter != m_cheFileDataMap.end(); iter++)
// 	{
// 		CStringA sOne;
// 		sOne.Format("%s, %d\n", iter->first.c_str(), iter->second->GetData()->nDataCnt);
// 		sTips += sOne;
// 	}
// 	MessageBoxA(GetSafeHwnd(), sTips, "test", 0);
}


BOOL CTeeChartDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam == IDC_BUTTONSAVE)
	{
		OnBnClickedButtonsave();
		return true;
	}
	else if (wParam == IDC_BUTTON_OUTDIR)
	{
		OnBnClickedButtonOutdir();
	}
	else if (wParam == IDC_BUTTON_RESET)
	{
		OnBnClickedButtonReset();
	}
	else if (wParam == IDC_BUTTON_FIX)
	{
		OnBnClickedButtonFix();
	}
	return __super::OnCommand( wParam,  lParam);
}

void CTeeChartDlg::OnBnClickedButtonsave()
{
	// TODO: Add your control notification handler code here
	if (m_pCurData)
	{		
// 		int n1 = GetDlgItemInt(IDC_EDIT_DATAFROM);
// 		int n2 = GetDlgItemInt(IDC_EDIT_DATATO);
// 		m_pCurData->ChangeDataRange(n1, n2);

		COleDateTime dtSave;
		SYSTEMTIME sysDate;
		SYSTEMTIME sysTime;
		m_dtDateCtrl.GetTime(&sysDate);
		m_dtTimeCtrl.GetTime(&sysTime);
		dtSave.SetDateTime(sysDate.wYear, sysDate.wMonth, sysDate.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		m_pCurData->ChangeTimes((DATE)dtSave);

		CString sOutPath;
		CString sOutName;
		GetDlgItemText(IDC_EDIT_OUTPUTPATH, sOutPath);
		GetDlgItemText(IDC_EDIT_NAME, sOutName);
		sOutPath += L"\\";
		sOutPath += sOutName;

		if (_taccess(sOutPath, 0) == 0)
		{
			if (MessageBox(L"目标文件已存在，确定进行覆盖吗？",L"提示", MB_OKCANCEL) == IDCANCEL)
			{
				return;
			}

			if (!DeleteFile(sOutPath))
			{

			}		
		}

		if(sOutPath.GetLength() > 0 && m_pCurData->SaveFile(sOutPath))
		{
			MessageBox(L"保存成功",L"提示");
			return;
		}
	}	
	MessageBox(L"保存失败",L"提示");
}

void CTeeChartDlg::ActiveTab(int Idx)
{
	ResizeTabs();
	m_tabControl.SetCurSel(Idx);
	m_list1.ShowWindow(Idx == 0);
	m_list2.ShowWindow(Idx == 1);
	//m_list3.ShowWindow(Idx == 2);
}

void CTeeChartDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	int n = m_tabControl.GetCurSel();
	ActiveTab(n);
}

CRect GetCtrlRect(HWND hDlg, HWND hSubWnd)
{
	if (hDlg == NULL || !IsWindow(hDlg))	return CRect(0,0,0,0);

	CRect rtClient;

	HWND hWnd = hSubWnd;
	if (hWnd == NULL || !IsWindow(hWnd))	return CRect(0,0,0,0);
	::GetWindowRect(hWnd, &rtClient);

	CWnd::FromHandle(hDlg)->ScreenToClient(&rtClient);

	return rtClient;
}

CRect GetCtrlRect(HWND hDlg, UINT nID)
{
	if (hDlg == NULL || !IsWindow(hDlg))	return CRect(0,0,0,0);

	HWND hWnd = ::GetDlgItem(hDlg, nID);

	return GetCtrlRect(hDlg, hWnd);
}

void CTeeChartDlg::ResizeTabs()
{
	if (m_list1.GetSafeHwnd())
	{
		CRect rcTab;
	//	m_tabControl.GetItemRect(0, &rcTab);
		m_tabControl.GetClientRect(&rcTab);
		//CRect rcTab = GetCtrlRect(GetSafeHwnd(), IDC_TAB1);
		rcTab.InflateRect(-2,-2,-2,-2);
		rcTab.top += 26;
		m_list1.MoveWindow(rcTab);
		m_list2.MoveWindow(rcTab);
		//m_list3.MoveWindow(rcTab);
	}
}

void CTeeChartDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	ResizeTabs();
}

void CTeeChartDlg::RefreshChart()
{
	if (!m_pCurData)
	{
		return;
	}

	auto & arrData = m_pCurData->GetData()->verMainDatas;
	int nDataCnt = arrData.size();
	((CSeries)m_chart.Series(0)).Clear();

	CSeries serDemo = (CSeries)m_chart.Series(0);

	for(long i=0;i<nDataCnt;i++)
	{
		double dval = 0;
		m_pCurData->GetDataByIdx(i,dval);
		double didx = m_pCurData->GetXValue(i);
		serDemo.AddNullXY(didx, dval, NULL );
	}
	long l = ((CSeries)m_chart.Series(0)).get_VerticalAxis();
	CAxis aleft = ((CAxes)m_chart.get_Axis()).get_Left();
	CAxis abottom = ((CAxes)m_chart.get_Axis()).get_Bottom();
	aleft.put_Minimum(0);
	aleft.put_Automatic(false);
	aleft.put_Maximum(m_pCurData->GetData()->nYLimit_High);
	aleft.put_Minimum(m_pCurData->GetData()->nYLimit_Low);
	aleft.put_Increment(1);

	abottom.put_Automatic(false);
	abottom.put_Minimum(0);
	abottom.put_Maximum(m_pCurData->GetXValue(m_pCurData->GetDataCnt()));
	abottom.put_Increment(0.0002);
 	abottom.put_StartPosition(0);
}


void CTeeChartDlg::OnBnClickedButtonOutdir()
{
	// TODO: Add your control notification handler code here

	//选择输出路径
	TCHAR szDir[MAX_PATH];
	BROWSEINFO bi; 
	ITEMIDLIST *pidl; 
	bi.hwndOwner = this->m_hWnd; 
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;//这个是输出缓冲区 
	bi.lpszTitle = _T("选择输出文件夹："); //标题
	bi.ulFlags = BIF_NEWDIALOGSTYLE;//使用新的界面,在win7中效果较好//BIF_RETURNONLYFSDIRS; 
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	pidl = SHBrowseForFolder(&bi);//弹出对话框 
	if (pidl == NULL)//点了取消，或者选择了无效的文件夹则返回NULL
		return;

	if(SHGetPathFromIDList(pidl, szDir)) 
	{
		SetDlgItemText(IDC_EDIT_OUTPUTPATH, szDir);
	}

}


void CTeeChartDlg::OnBnClickedButtonReset()
{
	// TODO: Add your control notification handler code here

//  	CString s1, s2, s3;
//  	GetDlgItemText(IDC_EDIT_DATAFROM, s1);
// 
// 	int nTimes = _ttoi((LPCTSTR)s1);
// 
// 	//m_pCurData->AppendTimes(nTimes);
// 	m_pCurData->RemoveWave(nTimes);
// 
// 	return;

// 	GetDlgItemText(IDC_EDIT_DATATO, s2);
// 	GetDlgItemText(IDC_EDIT_DATA3, s3);
// 
// 	double fV  = _ttof((LPCTSTR)s1);
// 	double fV2 = _ttof((LPCTSTR)s2);
// 	double fV3 = _ttof((LPCTSTR)s3);
// 
// 	double fr = 0;
// 	if (s1.GetLength() > 0 && s2.GetLength() > 0)
// 	{
// 		fr = 2.05 * (fV/0.95)*((21-6)/(21-fV2)); 
// 	}
// 	else if (s1.GetLength() > 0 && s3.GetLength() > 0)
// 	{
// 		fr = 21- 15 / (fV3 / (2.05 * (fV/0.95))); 
// 	}
// 
// 	CString sText;
// 	sText.Format(L"%f", fr);
// 	AfxMessageBox(sText);
// 	return;

	//NOx=2.05 x (NO/0.95) x [(21-6)/(21-O2)] 
	//NOx=1.53 x NO + NO2 
	if (m_pCurData)
	{
		if (MessageBox(L"确定要恢复原波形数据吗？", L"提示", MB_YESNO) == IDNO)
		{
			return;
		}
		LoadData();
	}
	else
	{
		MessageBox(L"没有找到需要恢复的数据", L"提示", 0);
	}
}


void CTeeChartDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnTimer(nIDEvent);

	if (m_pListRecover)
	{
		m_pListRecover->SetItemText(m_nRowRecover, m_nColRecover, m_strTextRecover);
		m_pListRecover = NULL;
	}
	KillTimer(nIDEvent);
}

bool CTeeChartDlg::GetWaveInfo(int nWaveIdx, float & fTimes, float & fWidth, int & nTopVal, float & f)
{
	if (!m_pCurData)
	{
		return false;
	}	

	sJfItem2 item;
	if(m_pCurData->GetWaveByIdx(nWaveIdx, item))
	{
		fTimes = m_pCurData->IdxToTime(item.nTopDataIdx); 
		
		fWidth = m_pCurData->IdxToTime(item.nEndDataIdx - item.nBeginDataIdx);
		
		nTopVal = item.nTopHVal;

		f = item.nTopSqrt;
	}

	return true;
}

bool CTeeChartDlg::OnAddNewWave(int nWaveIdx, float fTimes, float fWidth, int nTopVal, float f)
{
	if (!m_pCurData)
	{
		return false;
	}

	if (m_pCurData->AddWave(fTimes, fWidth, nTopVal, f, nWaveIdx) >= 0)
	{
		RebuildUI();
		return true;
	}

	if (m_pCurData->GetLastErr().GetLength() > 0)
	{	
		AfxMessageBox(m_pCurData->GetLastErr());
	}

	return false;
}

bool CTeeChartDlg::OnChangeTimesNotify(float fTime)
{
	if (!m_pCurData)
	{
		if (m_pCurData->GetLastErr().GetLength() > 0)
		{
			AfxMessageBox(m_pCurData->GetLastErr());
		}
		return false;
	}

	m_pCurData->AppendTimes(fTime);
	RefreshChart();
	return true;
}

void CTeeChartDlg::OnBnClickedButtonRemovewave()
{
	// TODO: Add your control notification handler code here
	if (!m_pCurData)
	{
		return;
	}

	POSITION ps = m_list1.GetFirstSelectedItemPosition();
	int nWaveIdx = m_list1.GetNextSelectedItem(ps);
	if (nWaveIdx >=0 && MessageBox(L"确定删除当前选择波形吗？", L"提示", MB_OKCANCEL) == IDOK)
	{
		m_pCurData->RemoveWave(nWaveIdx);

		RebuildUI();
	}
}


void CTeeChartDlg::OnBnClickedButtonAddwave()
{
	// TODO: Add your control notification handler code here

	if (!m_pCurData)
	{
		return;
	}

	vector<CString> arrWaves;
	for (int i = 0; i < m_pCurData->GetWaveCnt(); i++)
	{
		CString sName;
		sName.Format(L"编号%d", i+1);
		arrWaves.push_back(sName);
	}

	CAddWaveDlg dlg(this, &arrWaves);
	if (dlg.DoModal() == IDOK)
	{

	}

}


void CTeeChartDlg::OnBnClickedButtonEdittimes()
{
	// TODO: Add your control notification handler code here
	if (!m_pCurData)
	{
		return;
	}

	CEditTimesDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{

	}
}


void CTeeChartDlg::OnBnClickedButtonFix()
{
	// TODO: Add your control notification handler code here
	if (!m_pCurData)
	{
		return;
	}

	TCHAR sFile[MAX_PATH] = {0};
	GetModuleFileName(NULL, sFile, MAX_PATH);
	CString sPath = GetPath(sFile);
	sPath += L"\\stdw.dat";

	POSITION ps = m_list1.GetFirstSelectedItemPosition();
	int nWaveIdx = m_list1.GetNextSelectedItem(ps);
	if (nWaveIdx >=0 && MessageBox(L"该操作不会改变峰高、峰宽、保留时间，会使波形变为平滑.\r\n确定进行该操作吗？", L"提示", MB_OKCANCEL) == IDOK)
	{
		USES_CONVERSION;
		if (_taccess(sPath, 0) != 0)
		{
#if _DEBUG
			m_pCurData->SaveWave(nWaveIdx, T2A(sPath));
#else
			AfxMessageBox(L"丢失资源文件，无法进行");
#endif
			return;
		}

		if(m_pCurData->FixWave(nWaveIdx,  T2A(sPath)))
		{
			RefreshChart();
		}
		else
		{
			if (m_pCurData->GetLastErr().GetLength() > 0)
			{
				AfxMessageBox(m_pCurData->GetLastErr());
			}
		}
	}

	return ;
}
