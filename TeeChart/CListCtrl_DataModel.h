#pragma once
#include <vector>
#include <algorithm>
using namespace std;

struct CListCtrl_DataRecord
{
	CListCtrl_DataRecord()
	{}

	CListCtrl_DataRecord(
	CString	sNumber,		//序号
	CString	sliveTime,	//保留时间
	CString	sTopSqrt,		//峰面积
	CString	sTopSqrtPer,	//峰面积比
	CString	sTopHVal,		//峰高
	CString	sTopWVal,		//峰宽
	CString sGroupName,	//组名
	CString sContentsVal,	//含量
	CString sTopStype		//峰形
	)
	{
		m_sNumber		= sNumber;		//序号
		m_sliveTime		= sliveTime;	//保留时间
		m_sTopSqrt		= sTopSqrt;		//峰面积
		m_sTopSqrtPer	= sTopSqrtPer;	//峰面积比
		m_sTopHVal		= sTopHVal;		//峰高
		m_sTopWVal		= sTopWVal;		//峰宽
		m_sGroupName	= sGroupName;	//组名
		m_sContentsVal	= sContentsVal;	//含量
		m_sTopStype		= sTopStype;	//峰形
	}

	CString	m_sNumber;		//序号
	CString	m_sliveTime;	//保留时间
	CString	m_sTopSqrt;		//峰面积
	CString	m_sTopSqrtPer;	//峰面积比
	CString	m_sTopHVal;		//峰高
	CString	m_sTopWVal;		//峰宽
	CString m_sGroupName;	//组名
	CString m_sContentsVal;	//含量
	CString m_sTopStype;	//峰形


	CString GetCellText(int col, bool title) const
	{
		switch(col)
		{
		case 0: { static const CString title0(_T("序号")); return title ? title0 : m_sNumber; }
		case 1: { static const CString title1(_T("保留时间")); return title ? title1 : m_sliveTime; }
		case 2: { static const CString title2(_T("峰面积")); return title ? title2 : m_sTopSqrt; }
		case 3: { static const CString title3(_T("峰面积比")); return title ? title3 : m_sTopSqrtPer; }
		case 4: { static const CString title0(_T("峰高")); return title ? title0 : m_sTopHVal; }
		case 5: { static const CString title1(_T("峰宽")); return title ? title1 : m_sTopWVal; }
		case 6: { static const CString title2(_T("组名")); return title ? title2 : m_sGroupName; }
		case 7: { static const CString title3(_T("含量")); return title ? title3 : m_sContentsVal; }
		case 8: { static const CString title3(_T("峰形")); return title ? title3 : m_sTopStype; }
		default:{ static const CString emptyStr; return emptyStr; }
		}
	}

	int  GetColCount() const { return 9; }
};

class CListCtrl_DataModel
{
	vector<CListCtrl_DataRecord> m_Records;
	int	m_LookupTime;
	int m_RowMultiplier;

public:
	CListCtrl_DataModel()
		:m_RowMultiplier(0)
		,m_LookupTime(0)
	{
		InitDataModel();
	}

	void InitDataModel()
	{
		m_Records.clear();
		if (m_RowMultiplier > 1)
		{
			vector<CListCtrl_DataRecord> rowset(m_Records);
			m_Records.reserve((m_RowMultiplier-1) * rowset.size());
			for(int i = 0 ; i < m_RowMultiplier ; ++i)
			{
				m_Records.insert(m_Records.end(), rowset.begin(), rowset.end());
			}
		}
	}

	CString GetCellText(size_t lookupId, int col) const
	{
		if (lookupId >= m_Records.size())
		{
			static CString oob(_T("Out of Bound"));
			return oob;
		}
		// How many times should we search sequential for the row ?
		for(int i=0; i < m_LookupTime; ++i)
		{
			for(size_t rowId = 0; rowId < m_Records.size(); ++rowId)
			{
				if (rowId==lookupId)
					break;
			}
		}
		return m_Records.at(lookupId).GetCellText(col, false);
	}

	void Clear(){m_Records.clear();}

	int AddItem(
		CString	sNumber,		//序号
		CString	sliveTime,		//保留时间
		CString	sTopSqrt,		//峰面积
		CString	sTopSqrtPer,	//峰面积比
		CString	sTopHVal,		//峰高
		CString	sTopWVal,		//峰宽
		CString sGroupName,		//组名
		CString sContentsVal,	//含量
		CString sTopStype		//峰形
		){
		m_Records.push_back( CListCtrl_DataRecord(sNumber, sliveTime, sTopSqrt, sTopSqrtPer, sTopHVal, 
			sTopWVal, sGroupName, sContentsVal, sTopStype));
		return m_Records.size() - 1;
	}

	size_t GetRowIds() const { return m_Records.size(); }
	int GetColCount() const { return CListCtrl_DataRecord().GetColCount(); }
	CString GetColTitle(int col) const { return CListCtrl_DataRecord().GetCellText(col, true); }

	vector<CListCtrl_DataRecord>& GetRecords() { return m_Records; }
	void SetLookupTime(int lookupTimes) { m_LookupTime = lookupTimes; }
	void SetRowMultiplier(int multiply) { if (m_RowMultiplier != multiply ) { m_RowMultiplier = multiply; InitDataModel(); } }
};