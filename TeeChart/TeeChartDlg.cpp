
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
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTeeChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART1, m_chart);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
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
END_MESSAGE_MAP()


// CTeeChartDlg 消息处理程序
void CTeeChartDlg::RefreshListControl()
{
	m_ListCtrl.DeleteAllItems();
	// Insert data into list-control by copying from datamodel
	int nItem = 0;
	for(size_t rowId = 0; rowId < m_DataModel.GetRowIds() ; ++rowId)
	{
		nItem = m_ListCtrl.InsertItem(++nItem, m_DataModel.GetCellText(rowId,0));
		m_ListCtrl.SetItemData(nItem, rowId);
		for(int col = 0; col < m_DataModel.GetColCount() ; ++col)
		{
			int nCellCol = col+1;	// +1 because of hidden column
			const CString& strCellText = m_DataModel.GetCellText(rowId, col);
			m_ListCtrl.SetItemText(nItem, nCellCol, strCellText);
			if (nCellCol==3)
			{
				// 				if (strCellText==_T(""))
				// 					m_ListCtrl.SetCellImage(nItem, nCellCol, nStateImageIdx);	// unchecked
				// 				else
				// 					m_ListCtrl.SetCellImage(nItem, nCellCol, nStateImageIdx+1);	// checked
			}
		}
		m_ListCtrl.SetCellImage(nItem, 1, nItem); // Assign flag-images
	}

}

void CTeeChartDlg::InitListControl()
{
// 	m_ImageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 1, 0);
// 	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGDEN));
// 	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGGERM));
// 	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGFRAN));
// 	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGGREEC));
// 	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGSWED));
// 	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_FLGSPAIN));
// 	int nStateImageIdx = CGridColumnTraitDateTime::AppendStateImages(m_ListCtrl, m_ImageList);	// Add checkboxes
// 	m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	// Give better margin to editors
	m_ListCtrl.SetCellMargin(1.2);
	CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;
	m_ListCtrl.SetDefaultRowTrait(pRowTrait);
	m_ListCtrl.EnableVisualStyles(true);

	// Create Columns
	m_ListCtrl.InsertHiddenLabelColumn();	// Requires one never uses column 0

	for(int col = 0; col < m_DataModel.GetColCount() ; ++col)
	{
		const CString& title = m_DataModel.GetColTitle(col);
		CGridColumnTrait* pTrait = NULL;
		if (true || col==0 || col==1)	// City
		{
			pTrait = new CGridColumnTraitEdit;
		}
		m_ListCtrl.InsertColumnTrait(col+1, title, LVCFMT_LEFT, 100, col, pTrait);
	}


	
// 	CViewConfigSectionWinApp* pColumnProfile = new CViewConfigSectionWinApp(_T("Sample List"));
// 	pColumnProfile->AddProfile(_T("Default"));
// 	pColumnProfile->AddProfile(_T("Special"));
// 	m_ListCtrl.SetupColumnConfig(pColumnProfile);

}

void CTeeChartDlg::UpdateModuleInfo(sCheData* pData)
{
	m_DataModel.Clear();

	sJfItem3 item3;
	item3.fContentsVal = 0;
	item3.sGroupName = L"";

	for (int i = 0; i < pData->sJfData2.verItems.size(); i++)
	{
		auto iter = pData->sJfData2.verItems[i];
		auto iter2 = pData->sJfData.verItems[i];
		sJfItem3 * pItem3 = i < pData->sJgData.verItems.size() ? &pData->sJgData.verItems[i] : &item3;

		CString sNumber;
		sNumber.Format(L"%d", iter.nIdx + 1);

		CString sliveTime;
		sliveTime.Format(L"%d", iter.nIdx);

		CString sTopSqrt;
		sTopSqrt.Format(L"%d", iter.nTopSqrt);

		CString sTopSqrtPer;
		sTopSqrtPer.Format(L"%.2f", iter.nTopSqrt * 100 / pData->fTopSqrtTotal);

		CString sTopHVal;
		sTopHVal.Format(L"%d", iter.nTopHVal);

		CString sTopWVal;
		sTopWVal.Format(L"%.3f", iter2.fTimeTo - iter2.fTimeFrom);

		CString sGroupName;
		sGroupName.Format(L"%s", pItem3->sGroupName);

		CString sContentsVal;
		sContentsVal.Format(L"%.4f", pItem3->fContentsVal);

		CString sTopStype;
		sTopStype.Format(L"%d", iter.nIdx);

		m_DataModel.AddItem(sNumber, sliveTime, sTopSqrt, sTopSqrtPer, sTopHVal, sTopWVal, sGroupName, sContentsVal, sTopStype );
	}
}

bool CTeeChartDlg::LoadData()
{
	CString sInput;
	GetDlgItemText(IDC_EDIT_INPUTPATH, sInput);
	
	if (!m_cheFileData.LoadFile(sInput))
	{
		return false;
	}

	UpdateModuleInfo(m_cheFileData.GetData());

	RefreshListControl();
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

	CSeries(m_chart.Series(0)).Add(0, _T("aaaaaaaa"), 1);


	CSeries serDemo = (CSeries)m_chart.Series(0);
	serDemo.AddNullXY(5,5,L"aaaabbb" );
 	serDemo.AddNullXY(3,7,NULL );
 	serDemo.AddNullXY(8,2,NULL );


	InitListControl();


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
