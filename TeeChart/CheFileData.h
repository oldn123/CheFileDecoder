#pragma once
#include <vector>
using namespace std;

struct sJfItem
{
	float fTimeFrom;
	float fTimeTo;
	float fPowerFrom;	//时间点对应的电压值
	float fPowerTo;		//时间点对应的电压值
	WORD  wUnKnow;		// =0
};

struct sItemData
{
	WORD wType;
	WORD wItemCnt;
	int nItemCnt;
	vector<sJfItem>	verItems;
};

struct sJfItem2
{
	WORD wUnKnow1;
	DWORD dwUnknow1;
	DWORD dwUnknow2;
	DWORD dwUnknow3;
	DWORD dwUnknow4;
	DWORD dwUnknow5;
	DWORD dwUnknow6;
	DWORD dwUnknow7;
	int	  nTopSqrt;
	int	  nTopHVal;
	WORD  nIdx;
	DWORD dwUnknow8;		// =0
};

struct sItemData2
{
	WORD wType;
	WORD wItemCnt;
	int nItemCnt;
	vector<sJfItem2>	verItems;
};


struct sJfItem3
{
	WORD wUnKnow1;
	DWORD dwUnknow1;
	DWORD dwUnknow2;
	DWORD dwUnknow3;
	DWORD dwUnknow4;
	DWORD dwUnknow5;
	DWORD dwUnknow6;
	DWORD dwUnknow7;
	int	  nTopSqrt;
	int	  nTopHVal;
	WORD  nIdx;
	DWORD dwUnknow8;		// =0
};

struct sItemData3
{
	WORD wType;
	WORD wItemCnt;
	int nItemCnt;
	vector<sJfItem3>	verItems;
};

struct sCheData
{
	char sVer[0x22];
	int nDataCnt;
	int nYLimit;
	int nUnKnowVal1;
	vector<DWORD>	verMainDatas;
	char bufUnKnow1[0x2E];
	DATE dtOle1;
	DATE dtOle2;
	int nUnKnowVal2;
	DATE dtOle3;
	DATE dtOle4;	//8byte 00 00 00 00 00 00 00 00 

	sItemData	sJfData;
	sItemData2	sJfData2;
	sItemData3  sJgData;	//结果数据
};

class CCheFileData
{
public:
	CCheFileData(void);
	~CCheFileData(void);

	bool OpenFile(LPCTSTR);

protected:
	sCheData	m_sCheData;
};

