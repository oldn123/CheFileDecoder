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
	WORD	wUnKnow1;
	int		nBeginDataIdx;			//波形起始数据的索引号	通过IdxToTime换算为时间
	int		nTopDataIdx;			//波形峰顶的索引号		通过IdxToTime换算为时间
	int		nEndDataIdx;			//波形结束数据的索引号	通过IdxToTime换算为时间

	DWORD	dwUnknow7;
	int		nTopSqrt;
	int		nTopHVal;
	int		nTopHPos;			//顶部的位置
	int		nTopHFrom;			//峰起始高度
	int		nTopHTo;			//峰终止高度
	WORD	nIdx;
	DWORD	dwUnknow8;		// =0
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
	float fTopSqrt;
	float fTopHVal;
	float fTopWVal;		//峰宽

	float fUnKonw3;
	float fUnKonw4;
	float fUnKonw5;

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
	void	ChangeTimes(DATE dtCreate);
	
	int		GetDataCnt();
	bool	GetDataByIdx(int, double & );
	bool	SetDataByIdx(int, double);
	double  GetXValue(int );

	void	ChangeDataCnt(int);
	void	ChangeDataRange(int, int);

	int		GetWaveCnt();
	bool	GetWaveByIdx(int, sJfItem & );
	bool	GetWaveLiveTimeByIdx(int, double & );
	bool	GetWaveTopByIdx(int, int & );
	bool	ChangeWaveTop(int, int );

	bool	ChangeWaveTimeRange(int, double tFrom, double tEnd);
	bool	ChangeWaveTimePos(int, double tLive );
	void	RemoveWave(int);

	CString GetLastErr(){return m_lastErr;}

protected:
	void	Clear();
	void	ZoomWave(int nIdx, sJfItem & item, float fZoom);
	int		TimeToIdx(double);
	double	IdxToTime(int);
	int		TestTimeRange(double dFrom, double dTo, int butIdx);
	int		GetRandomVal(int, int, int nIdx, int nTimeRange);

protected:
	sCheData	m_sCheData;
	CString		m_strInputfile;

protected:
	int			m_nSaveDataFromIdx;
	CString		m_lastErr;
};

