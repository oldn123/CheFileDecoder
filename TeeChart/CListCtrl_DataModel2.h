#pragma once
#include <vector>
#include <algorithm>
using namespace std;

struct CListCtrl_DataRecord2
{
	CListCtrl_DataRecord2()
	{}

	CListCtrl_DataRecord2(
	CString	sNumber,		//���
	CString sGroupName,		//����
	CString	sliveTime,		//����ʱ��
	CString	sTopSqrt,		//�����
	CString	sTopHVal,		//���
	CString sContentsVal,	//����
	CString sTopStype		//����
	)
	{
		m_sNumber		= sNumber;		//���
		m_sliveTime		= sliveTime;	//����ʱ��
		m_sTopSqrt		= sTopSqrt;		//�����
		m_sTopHVal		= sTopHVal;		//���
		m_sGroupName	= sGroupName;	//����
		m_sContentsVal	= sContentsVal;	//����
		m_sTopStype		= sTopStype;	//����
	}

	CString	m_sNumber;		//���
	CString	m_sliveTime;	//����ʱ��
	CString	m_sTopSqrt;		//�����
	CString	m_sTopHVal;		//���
	CString m_sGroupName;	//����
	CString m_sContentsVal;	//����
	CString m_sTopStype;	//����


	CString GetCellText(int col, bool title, bool & bCanEdit) const
	{
		bCanEdit = false;
		switch(col)
		{
		case 0: { static const CString title0(_T("���")); return title ? title0 : m_sNumber; }
		case 1: { static const CString title2(_T("����")); bCanEdit = false;return title ? title2 : m_sGroupName; }
		case 2: { static const CString title1(_T("����ʱ��")); bCanEdit = false;return title ? title1 : m_sliveTime; }
		case 3: { static const CString title2(_T("�����")); bCanEdit = false;return title ? title2 : m_sTopSqrt; }
		case 4: { static const CString title0(_T("���")); bCanEdit = false;return title ? title0 : m_sTopHVal; }
		case 5: { static const CString title3(_T("����")); bCanEdit = false;return title ? title3 : m_sContentsVal; }
		case 6: { static const CString title3(_T("����")); return title ? title3 : m_sTopStype; }
		default:{ static const CString emptyStr; return emptyStr; }
		}
	}

	int  GetColCount() const { return 6; }
};

class CListCtrl_DataModel2 : public IDataModelForList
{
	vector<CListCtrl_DataRecord2> m_Records;
	int	m_LookupTime;
	int m_RowMultiplier;

public:
	CListCtrl_DataModel2()
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
			vector<CListCtrl_DataRecord2> rowset(m_Records);
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
		bool bCanEdit =  false;
		return m_Records.at(lookupId).GetCellText(col, false, bCanEdit);
	}

	void Clear(){m_Records.clear();}

	int AddItem(
		CString	sNumber,		//���
		CString sGroupName,		//����
		CString	sliveTime,		//����ʱ��
		CString	sTopSqrt,		//�����
		CString	sTopHVal,		//���
		CString sContentsVal,	//����
		CString sTopStype		//����
		){
		m_Records.push_back( CListCtrl_DataRecord2(sNumber, sGroupName, sliveTime, sTopSqrt, sTopHVal, sContentsVal, sTopStype));
		return m_Records.size() - 1;
	}

	size_t GetRowIds() const { return m_Records.size(); }
	int GetColCount() const { return CListCtrl_DataRecord2().GetColCount(); }
	CString GetColTitle(int col, bool & bCanEdit) const { return CListCtrl_DataRecord2().GetCellText(col, true, bCanEdit); }

	vector<CListCtrl_DataRecord2>& GetRecords() { return m_Records; }
	void SetLookupTime(int lookupTimes) { m_LookupTime = lookupTimes; }
	void SetRowMultiplier(int multiply) { if (m_RowMultiplier != multiply ) { m_RowMultiplier = multiply; InitDataModel(); } }
};