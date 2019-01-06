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
	CString sGroupName;
	float fContentsVal;	//含量
	float fLiveTime;	//保留时间
	float fLivePower;	//保留时间对应的电压
	int   nIdx1;
	int   nIdx2;

	float fUnKonw1;
	float fUnKonw2;
	float fUnKonw3;
	float fUnKonw4;
	float fUnKonw5;
	float fUnKonw6;
	float fUnKonw7;
	float fUnKonw8;

	DWORD dwUnkown1;		//0
	DWORD dwUnkown2;		//-1
};

struct sItemData3
{
	WORD wType;
	WORD wItemCnt;
	int nItemCnt;
	BYTE unKownBytes_0a[0x0a];
	vector<sJfItem3>	verItems;
};

struct sCheData
{
	char sVer[0x22];
	int nDataCnt;
	int nYLimit_High;
	int nYLimit_Low;
	BYTE unKownBytes_0a[0x0a];
	vector<DWORD>	verMainDatas;
	BYTE unKownBytes_2e[0x2E];
	DATE dtOle1;
	DATE dtOle2;
	int nUnKnowVal2;
	DATE dtOle3;
	DATE dtOle4;	//8byte 00 00 00 00 00 00 00 00 

	sItemData	sJfData;
	sItemData2	sJfData2;
	sItemData3  sJgData;	//结果数据
	BYTE unKownBytes_aa[0xaa];
	//
	float fTopSqrtTotal;
};

class CCheFileData
{
public:
	CCheFileData(void);
	~CCheFileData(void);

	bool LoadFile(LPCTSTR);

	bool SaveFile(LPCTSTR);

	sCheData * GetData(){return &m_sCheData;}

public:
	int		GetDataCnt();
	bool	GetDataByIdx(int, double & );
	bool	SetDataByIdx(int, double);
	double  GetXValue(int );

	void	ChangeDataCnt(int);
	void	ChangeDataRange(int, int);

protected:
	void Clear();

protected:
	sCheData	m_sCheData;
	CString		m_strInputfile;

protected:
	int			m_nSaveDataFromIdx;
};

