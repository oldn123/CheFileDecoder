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
	WORD	wUnKnow2;			// =0
	WORD	wUnKnow3;			// =0
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
	BYTE unKownBytes_06[0x06];
	BYTE unKownBytes_08[0x08];
	vector<sJfItem3>	verItems;
};

struct sCheData
{
	char sVer[0x22];
	int nDataCnt;
	int *nDataCnt2;
	int nYLimit_High;
	int nYLimit_Low;
	BYTE unKownBytes_0a[0x0a];
	vector<DWORD>	verMainDatas;
	/****/	
	BYTE unKownBytes_18[0x18];
	WORD wNameLen;
	char sBuff[0x10];
	BYTE unKownBytes_14[0x14];
	/****/
	DATE dtOle1;
	DATE dtOle2;
	int nUnKnowVal2;
	DATE dtOle3;
	DATE dtOle4;	//8byte 00 00 00 00 00 00 00 00 

	sItemData	sJfData;
	sItemData2	sJfData2;
	sItemData3  sJgData;	//�������
	BYTE unKownBytes_5e[0x5e];
	BYTE unKownBytes_end[0x100];	//0x54 , 0x4c �䳤��β���ݣ�û��ϸ���������ӱ����洢����
	//
	int	nEndByesCnt;
	float fTopSqrtTotal;
};

struct sWaveInfo
{
	sJfItem2		item;
	vector<DWORD>	datas;
};

class CCheFileData
{
public:
	CCheFileData(void);
	~CCheFileData(void);

	static void DoInit();
	static void DoUninit();

	bool LoadFile(LPCTSTR);
	bool SaveFile(LPCTSTR);

	sCheData * GetData(){return &m_sCheData;}

public:
	bool	SaveWave(int, const char *);
	bool	LoadWave(const char*, sWaveInfo &);

	void	ChangeTimes(DATE dtCreate);
	static long	DataToValue(long);
	static long	ValueToData(long);

	static int	TimeToIdx(double);
	static double IdxToTime(int);

	int		GetDataCnt();
	bool	GetDataByIdx(int, double & );
	bool	SetDataByIdx(int, double);
	double  GetXValue(int );

	void	ChangeDataCnt(int);
	void	ChangeDataRange(int, int);

	int		GetWaveCnt();
	bool	GetWaveByIdx(int, sJfItem2 & );
	bool	GetWaveLiveTimeByIdx(int, double & );
	bool	GetWaveTopByIdx(int, int & );

	bool	ChangeWaveSqrt(int, int );
	bool	ChangeWaveTop(int, int );
	bool	ChangeWaveTimeWidth(int, double tWidth, sWaveInfo * pInputWave = NULL);
	bool	ChangeWaveTimeRange(int, double tFrom, double tEnd, sWaveInfo * pInputWave = NULL);
	bool	ChangeWaveTimePos(int, double tLive, bool bcopyMode = false);

	void	NormalizeWave(int);
	void	FixWaveEdge(int);
	bool	FixWave(int, const char* sFile);
	void	SmoothWave(int);
	bool	ResetStdWave(int nWaveIdx, int nFromIdx, int nToIdx, long lTop);
	CString GetLastErr(){return m_lastErr;}

	void	RemoveWave(int nIdx);
	int		AddWave(double tLiveTime, double tWidth, int nTopValue , double dSqrt, int nBaseOnIdx);
	void	AppendDatas(int nDataCnt);
	void	AppendTimes(float nSecs);
protected:
	void	Clear();
	void	ZoomWave(int nIdx, float fZoom);
	void	FillStep(int nFromIdx, int nToIdx, int nTopFrom, int nTopTo);

	int		TestTimeRange(double dFrom, double dTo, int butIdx);
	int		GetRandomVal(int, int, int nIdx, int nTimeRange);
	long	OffsetHPos(vector<DWORD>& arrdatas, int nMidPos, int nPosOffset1, int nPosOffset2, double fZoom = 1.0);

	void	MakeLineVal(int nfrom, int nto, int ncnt, bool bHasLeft, bool bHasRigth, vector<int> & arr);
	void	_ZoomTimeData(vector<int> & arrInData, int nNewCnt, vector<int> & arrOutData);
	void	FixWaveBottomLine(int);
protected:
	sCheData	m_sCheData;
	CString		m_strInputfile;

protected:
	int			m_nSaveDataFromIdx;
	CString		m_lastErr;
	int			m_nNorLineWidthStep;	
	int			m_nNorLineHeightStep;
};

