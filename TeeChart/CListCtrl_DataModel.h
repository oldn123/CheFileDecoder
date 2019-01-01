#pragma once
#include <vector>
#include <algorithm>
using namespace std;

struct CListCtrl_DataRecord
{
	CListCtrl_DataRecord()
	{}

	CListCtrl_DataRecord(
	CString	sNumber,		//���
	CString	sliveTime,	//����ʱ��
	CString	sTopSqrt,		//�����
	CString	sTopSqrtPer,	//�������
	CString	sTopHVal,		//���
	CString	sTopWVal,		//���
	CString sGroupName,	//����
	CString sContentsVal,	//����
	CString sTopStype		//����
	)
	{
		m_sNumber		= sNumber;		//���
		m_sliveTime		= sliveTime;	//����ʱ��
		m_sTopSqrt		= sTopSqrt;		//�����
		m_sTopSqrtPer	= sTopSqrtPer;	//�������
		m_sTopHVal		= sTopHVal;		//���
		m_sTopWVal		= sTopWVal;		//���
		m_sGroupName	= sGroupName;	//����
		m_sContentsVal	= sContentsVal;	//����
		m_sTopStype		= sTopStype;	//����
	}

	CString	m_sNumber;		//���
	CString	m_sliveTime;	//����ʱ��
	CString	m_sTopSqrt;		//�����
	CString	m_sTopSqrtPer;	//�������
	CString	m_sTopHVal;		//���
	CString	m_sTopWVal;		//���
	CString m_sGroupName;	//����
	CString m_sContentsVal;	//����
	CString m_sTopStype;	//����


	CString GetCellText(int col, bool title) const
	{
		switch(col)
		{
		case 0: { static const CString title0(_T("���")); return title ? title0 : m_sNumber; }
		case 1: { static const CString title1(_T("����ʱ��")); return title ? title1 : m_sliveTime; }
		case 2: { static const CString title2(_T("�����")); return title ? title2 : m_sTopSqrt; }
		case 3: { static const CString title3(_T("�������")); return title ? title3 : m_sTopSqrtPer; }
		case 4: { static const CString title0(_T("���")); return title ? title0 : m_sTopHVal; }
		case 5: { static const CString title1(_T("���")); return title ? title1 : m_sTopWVal; }
		case 6: { static const CString title2(_T("����")); return title ? title2 : m_sGroupName; }
		case 7: { static const CString title3(_T("����")); return title ? title3 : m_sContentsVal; }
		case 8: { static const CString title3(_T("����")); return title ? title3 : m_sTopStype; }
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
		CString	sNumber,		//���
		CString	sliveTime,		//����ʱ��
		CString	sTopSqrt,		//�����
		CString	sTopSqrtPer,	//�������
		CString	sTopHVal,		//���
		CString	sTopWVal,		//���
		CString sGroupName,		//����
		CString sContentsVal,	//����
		CString sTopStype		//����
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