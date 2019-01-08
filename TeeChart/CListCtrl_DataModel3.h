#pragma once
#include <vector>
#include <algorithm>
using namespace std;

struct CListCtrl_DataRecord3
{
	CListCtrl_DataRecord3()
	{}

	CListCtrl_DataRecord3(
	CString	sNumber,		//ÐòºÅ
	vector<CString> & sDatas
	)
	{
		m_sNumber		= sNumber;		//ÐòºÅ
		for (int i = 0; i < 10; i++)
		{
			sData[i] = sDatas[i];
		}
	}

	CString	m_sNumber;		//ÐòºÅ
	CString	sData[10];	//±£ÁôÊ±¼ä


	CString GetCellText(int col, bool title, bool & bCanEdit) const
	{
		bCanEdit = true;
		switch(col)
		{
		case 0: { static const CString title0(_T("ÐòºÅ")); return title ? title0 : m_sNumber; bCanEdit = false;}
		case 1: { static const CString title1(_T("1")); return title ? title1 : sData[0]; }
		case 2: { static const CString title2(_T("2")); return title ? title2 : sData[1]; }
		case 3: { static const CString title3(_T("3")); return title ? title3 : sData[2]; }
		case 4: { static const CString title0(_T("4")); return title ? title0 : sData[3]; }
		case 5: { static const CString title1(_T("5")); return title ? title1 : sData[4]; }
		case 6: { static const CString title2(_T("6")); return title ? title2 : sData[5]; }
		case 7: { static const CString title3(_T("7")); return title ? title3 : sData[6]; }
		case 8: { static const CString title3(_T("8")); return title ? title3 : sData[7]; }
		case 9: { static const CString title3(_T("9")); return title ? title3 : sData[8]; }
		case 10: { static const CString title3(_T("10")); return title ? title3 : sData[9]; }
		default:{ static const CString emptyStr; return emptyStr; }
		}
	}

	int  GetColCount() const { return 11; }
};

class CListCtrl_DataModel3 : public IDataModelForList
{
	vector<CListCtrl_DataRecord3> m_Records;
	int	m_LookupTime;
	int m_RowMultiplier;

public:
	CListCtrl_DataModel3()
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
			vector<CListCtrl_DataRecord3> rowset(m_Records);
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
		bool b;
		return m_Records.at(lookupId).GetCellText(col, false, b);
	}

	void Clear(){m_Records.clear();}

	int AddItem(
		CString	sNumber,		//ÐòºÅ
		vector<CString> & sDatas
		){
		m_Records.push_back( CListCtrl_DataRecord3(sNumber, sDatas));
		return m_Records.size() - 1;
	}

	size_t GetRowIds() const { return m_Records.size(); }
	int GetColCount() const { return CListCtrl_DataRecord3().GetColCount(); }
	CString GetColTitle(int col, bool & bCanEdit) const { return CListCtrl_DataRecord3().GetCellText(col, true, bCanEdit); }

	vector<CListCtrl_DataRecord3>& GetRecords() { return m_Records; }
	void SetLookupTime(int lookupTimes) { m_LookupTime = lookupTimes; }
	void SetRowMultiplier(int multiply) { if (m_RowMultiplier != multiply ) { m_RowMultiplier = multiply; InitDataModel(); } }
};