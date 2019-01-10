#pragma once
#include <vector>
using namespace std;

struct sJfItem
{
	float fTimeFrom;
	float fTimeTo;
	float fPowerFrom;	//ʱ����Ӧ�ĵ�ѹֵ
	float fPowerTo;		//ʱ����Ӧ�ĵ�ѹֵ
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
	int		nBeginDataIdx;			//������ʼ���ݵ�������	ͨ��IdxToTime����Ϊʱ��
	int		nTopDataIdx;			//���η嶥��������		ͨ��IdxToTime����Ϊʱ��
	int		nEndDataIdx;			//���ν������ݵ�������	ͨ��IdxToTime����Ϊʱ��

	DWORD	dwUnknow7;
	int		nTopSqrt;
	int		nTopHVal;
	int		nTopHPos;			//������λ��
	int		nTopHFrom;			//����ʼ�߶�
	int		nTopHTo;			//����ֹ�߶�
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
	float fContentsVal;	//����
	float fLiveTime;	//����ʱ��
	float fLivePower;	//����ʱ���Ӧ�ĵ�ѹ
	int   nIdxMatch;	//ƥ�����item
	int   nIdx;
	float fTopSqrt;
	float fTopHVal;
	float fTopWVal;		//���

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
	sItemData3  sJgData;	//�������
	BYTE unKownBytes_5e[0x5e];
	BYTE unKownBytes_4c[0x4c];
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

