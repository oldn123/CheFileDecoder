#include "StdAfx.h"
#include "CheFileData.h"
#include <assert.h>
#include <math.h>

CCheFileData::CCheFileData(void)
{
	Clear();
}


CCheFileData::~CCheFileData(void)
{
}

bool CCheFileData::SaveFile(LPCTSTR sFile)
{
	USES_CONVERSION;
	FILE * fp = fopen(T2A(sFile), "wb");
	if (!fp)
	{
		return false;
	}

	do 
	{
		{
			char sBuf[0x38] = {0};
			strcpy(sBuf, "CHERI10 ");

			*(int*)&sBuf[0x22] = m_sCheData.nDataCnt;
			*(int*)&sBuf[0x26] = m_sCheData.nYLimit_High;
			*(int*)&sBuf[0x2A] = m_sCheData.nYLimit_Low;
			memcpy(&sBuf[0x2E],m_sCheData.unKownBytes_0a,10);
			fwrite(sBuf, 1, 0x38, fp);
		}

		float ftotal = 0;
		if (m_sCheData.nDataCnt > 0)
		{	
			assert(m_sCheData.nDataCnt <= m_sCheData.verMainDatas.size());
			LPBYTE pDataBuf = (LPBYTE)m_sCheData.verMainDatas.data();
			fwrite(pDataBuf + m_nSaveDataFromIdx*4, 4, m_sCheData.nDataCnt, fp);
		}
		else
		{
			assert(0);
		}
		
		*(int*)&m_sCheData.unKownBytes_2e[6] = m_sCheData.nDataCnt;
		fwrite(m_sCheData.unKownBytes_2e, 1, 0x2E, fp);

		{
			char sBuf[0x2C] = {0};
			assert(m_sCheData.sJfData.wType == 3);

			*(DATE*)&sBuf[0] = (DATE)m_sCheData.dtOle1;
			*(DATE*)&sBuf[8] = (DATE)m_sCheData.dtOle2;
			*(int*)&sBuf[0x10] = m_sCheData.nUnKnowVal2;
			*(DATE*)&sBuf[0x14] = (DATE)m_sCheData.dtOle3;
			*(DATE*)&sBuf[0x1C] = (DATE)m_sCheData.dtOle4;

			*(WORD*)&sBuf[0x24] = m_sCheData.sJfData.wType;
			*(WORD*)&sBuf[0x26] = m_sCheData.sJfData.wItemCnt;
			*(DWORD*)&sBuf[0x28] = m_sCheData.sJfData.nItemCnt;
			fwrite(sBuf, 1, 0x2C, fp);
		}

		if (m_sCheData.sJfData.wItemCnt > 0)
		{
			{
				char * pJfDatas = new char[0x41 * 0x12];
				memset(pJfDatas, 0, 0x41 * 0x12);
				assert(m_sCheData.sJfData.nItemCnt == m_sCheData.sJfData.verItems.size());
				for(int i = 0; i < m_sCheData.sJfData.nItemCnt; i++)
				{
					sJfItem & si = m_sCheData.sJfData.verItems[i];
					*(float*)(&pJfDatas[i * 0x12] + 0) = si.fTimeFrom;
					*(float*)(&pJfDatas[i * 0x12] + 4) = si.fTimeTo;
					*(float*)(&pJfDatas[i * 0x12] + 8) = si.fPowerFrom;
					*(float*)(&pJfDatas[i * 0x12] + 0x0C) = si.fPowerTo;
					*(WORD*)(&pJfDatas[i * 0x12] + 0x10) = si.wUnKnow;

					assert(si.wUnKnow == 0);
				}
				fwrite(pJfDatas, 0x12, 0x41, fp);	//总量
				delete [] pJfDatas;
			}

			{
				char * pJfDatas2 = new char[0x41 * 0x2C + 8];
				memset(pJfDatas2, 0, 0x41 * 0x2C + 8);

				*(WORD*)&pJfDatas2[0] = m_sCheData.sJfData2.wType;
				*(WORD*)&pJfDatas2[2] = m_sCheData.sJfData2.wItemCnt;
				*(DWORD*)&pJfDatas2[4] = m_sCheData.sJfData2.nItemCnt;
				assert(m_sCheData.sJfData2.nItemCnt == m_sCheData.sJfData2.verItems.size());

				for(int i = 0; i < m_sCheData.sJfData2.nItemCnt; i++)
				{
					int nItemFrom = 8 + i * 0x2C;
					sJfItem2 & si = m_sCheData.sJfData2.verItems[i];

					*(WORD*)(&pJfDatas2[nItemFrom] + 0) = si.wUnKnow1;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 2)= si.nBeginDataIdx;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 6)= si.nTopDataIdx;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 0x0A)	= si.nEndDataIdx;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 0x0E)	= si.nTopHFrom;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 0x12)	= si.nTopHPos;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 0x16)	= si.nTopHTo;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 0x1A)	= si.dwUnknow7;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 0x1E)	= si.nTopSqrt;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 0x22)	= si.nTopHVal;
					*(WORD*)(&pJfDatas2[nItemFrom] + 0x26)	= si.nIdx;
					*(DWORD*)(&pJfDatas2[nItemFrom] + 0x28)	= si.dwUnknow8;		// =0
					assert(si.dwUnknow8 == 0);
				}
				fwrite(pJfDatas2, 1, 0x41 * 0x2C + 8, fp);	//总量
				delete [] pJfDatas2;
			}
		}
		
		{
			char buf[0x0a] = {0};
			char * pBuf = &buf[0];

			*(WORD*)&pBuf[0] = m_sCheData.sJgData.wType;
			*(WORD*)&pBuf[2] = m_sCheData.sJgData.wItemCnt;
			memcpy(&pBuf[4], m_sCheData.sJgData.unKownBytes_06, 0x06);
			fwrite(pBuf, 1, 0x0a, fp);	

			if (m_sCheData.sJgData.wItemCnt > 0)
			{
				char _buf[0x08] = {0};
				pBuf = &_buf[0];
				memcpy(&pBuf[0], m_sCheData.sJgData.unKownBytes_08, 0x08);
				fwrite(pBuf, 1, 0x08, fp);	

				assert(m_sCheData.sJgData.wItemCnt == m_sCheData.sJgData.verItems.size());
				USES_CONVERSION;
				for (int i = 0; i < m_sCheData.sJgData.wItemCnt; i++)
				{
					sJfItem3 & sItem = m_sCheData.sJgData.verItems.at(i);
					WORD wNameCnt = ((CStringA)sItem.sGroupName).GetLength();
					fwrite(&wNameCnt, 1, 2, fp);	//总量

					fwrite(((CStringA)sItem.sGroupName).GetBuffer(), 1, wNameCnt, fp);

					char sTmp[0x3C] = {0};

					*(float*)&sTmp[0x08] = sItem.fLiveTime;		//保留时间

					*(float*)&sTmp[0x0C] = sItem.fContentsVal;	

					*(float*)&sTmp[0x1C] = sItem.fLivePower;	//保留时间对应的电压
					*(int*)&sTmp[0x28] = sItem.nIdxMatch;	//	
					*(int*)&sTmp[0x2C] = sItem.nIdx;	//

					*(float*)&sTmp[0x00] = sItem.fTopSqrt;
					*(float*)&sTmp[0x04] = sItem.fTopHVal;
					*(float*)&sTmp[0x10] = sItem.fUnKonw3;
					*(float*)&sTmp[0x14] = sItem.fUnKonw4;
					*(float*)&sTmp[0x18] = sItem.fUnKonw5;
					*(float*)&sTmp[0x20] = sItem.fTopWVal;
					*(float*)&sTmp[0x24] = sItem.fUnKonw7;

					*(DWORD*)&sTmp[0x30] = sItem.dwUnkown1;		//0
					*(DWORD*)&sTmp[0x34] = sItem.dwUnkown2;		//-1
					assert(sItem.dwUnkown1 == 0);
					assert(sItem.dwUnkown2 == 0xffffffff);

					*(float*)&sTmp[0x38] = sItem.fUnKonw8;

					fwrite(sTmp, 1, 0x3C, fp);
				}
				fwrite(m_sCheData.unKownBytes_5e, 1, 0x5e, fp);
			}		
		}		
		assert(m_sCheData.nEndByesCnt != 0);
		fwrite(m_sCheData.unKownBytes_end, 1, m_sCheData.nEndByesCnt, fp);

	} while (false);

	fclose(fp);
	return true;
}

bool CCheFileData::LoadFile(LPCTSTR sInput)
{
	USES_CONVERSION;
	FILE * fp = fopen(CT2A((LPCTSTR)sInput), "rb");

	if (!fp)
	{
		return false;
	}
	Clear();
	do 
	{
		{
			char sBuf[0x38] = {0};
			fread(sBuf, 1, 0x38, fp);

			memcpy(m_sCheData.sVer, sBuf, 0x22);

			if (strcmp(m_sCheData.sVer, "CHERI10 ") != 0)
			{
				break;
			}

			m_sCheData.nDataCnt = *(int*)&sBuf[0x22];
			m_sCheData.nYLimit_High = *(int*)&sBuf[0x26];
			m_sCheData.nYLimit_Low = *(int*)&sBuf[0x2A];
			memcpy(m_sCheData.unKownBytes_0a, &sBuf[0x2E], 10);
		}

		if (m_sCheData.nDataCnt > 0)
		{
			DWORD * sDataBuf = new DWORD[m_sCheData.nDataCnt];
			fread(sDataBuf, 4, m_sCheData.nDataCnt, fp);

			for (int i = 0; i < m_sCheData.nDataCnt; i++)
			{
				m_sCheData.verMainDatas.push_back(sDataBuf[i]);
			}
			delete [] sDataBuf;
		}
		else
		{
			assert(0);
		}

		fread(m_sCheData.unKownBytes_2e, 1, 0x2E, fp);

		assert(*(DWORD*)&m_sCheData.unKownBytes_2e[0] == 0x1000c);	//0c 00 01 00
		assert(*(DWORD*)&m_sCheData.unKownBytes_2e[0x2e-4] == 0x10000e);	//0e 00 01 00

		{
			char sBuf[0x2C] = {0};
			fread(sBuf, 1, 0x2C, fp);

			m_sCheData.dtOle1 = *(DATE*)&sBuf[0];
			m_sCheData.dtOle2 = *(DATE*)&sBuf[8];
			m_sCheData.nUnKnowVal2 = *(int*)&sBuf[0x10];
			m_sCheData.dtOle3 = *(DATE*)&sBuf[0x14];
			m_sCheData.dtOle4 = *(DATE*)&sBuf[0x1C];

			m_sCheData.sJfData.wType = *(WORD*)&sBuf[0x24];
			m_sCheData.sJfData.wItemCnt = *(WORD*)&sBuf[0x26];
			m_sCheData.sJfData.nItemCnt = *(DWORD*)&sBuf[0x28];
		}

		assert(m_sCheData.sJfData.wType == 3);

		if (m_sCheData.sJfData.wItemCnt > 0)
		{
			char * pJfDatas = new char[0x41 * 0x12];

			fread(pJfDatas, 0x12, 0x41, fp);	//总量
			
			for(int i = 0; i < m_sCheData.sJfData.wItemCnt; i++)
			{
				sJfItem si;
				si.fTimeFrom = *(float*)(&pJfDatas[i * 0x12] + 0);
				si.fTimeTo = *(float*)(&pJfDatas[i * 0x12] + 4);
				si.fPowerFrom = *(float*)(&pJfDatas[i * 0x12] + 8);
				si.fPowerTo = *(float*)(&pJfDatas[i * 0x12] + 0x0C);
				si.wUnKnow = *(WORD*)(&pJfDatas[i * 0x12] + 0x10);

				assert(si.wUnKnow == 0);

				m_sCheData.sJfData.verItems.push_back(si);
			}

			delete [] pJfDatas;
			
			char * pJfDatas2 = new char[0x41 * 0x2C + 8];

			fread(pJfDatas2, 1, 0x41 * 0x2C + 8, fp);	//总量

			m_sCheData.sJfData2.wType = *(WORD*)&pJfDatas2[0];
			m_sCheData.sJfData2.wItemCnt = *(WORD*)&pJfDatas2[2];
			m_sCheData.sJfData2.nItemCnt = *(DWORD*)&pJfDatas2[4];
			m_sCheData.fTopSqrtTotal = 0;
			for(int i = 0; i < m_sCheData.sJfData2.wItemCnt; i++)
			{
				int nItemFrom = 8 + i * 0x2C;
				sJfItem2 si;
				si.wUnKnow1 = *(WORD*)(&pJfDatas2[nItemFrom] + 0);
				si.nBeginDataIdx = *(int*)(&pJfDatas2[nItemFrom] + 2);
				si.nTopDataIdx= *(int*)(&pJfDatas2[nItemFrom] + 6);
				si.nEndDataIdx= *(int*)(&pJfDatas2[nItemFrom] + 0x0A);

				si.nTopHFrom= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x0E);
				si.nTopHPos = *(DWORD*)(&pJfDatas2[nItemFrom] + 0x12);
				si.nTopHTo= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x16);
				si.dwUnknow7= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x1A);
				si.nTopSqrt= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x1E);
				si.nTopHVal= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x22);
				si.nIdx= *(WORD*)(&pJfDatas2[nItemFrom] + 0x26);
				si.dwUnknow8= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x28);		// =0

				assert(si.dwUnknow8 == 0);

				m_sCheData.fTopSqrtTotal += si.nTopSqrt;
				m_sCheData.sJfData2.verItems.push_back(si);
			}

			delete [] pJfDatas2;
		}

		{
			char buf[0x0a] = {0};
			char * pBuf = &buf[0];

			fread(pBuf, 1, 0x0a, fp);	

			m_sCheData.sJgData.wType = *(WORD*)&pBuf[0];
			assert(m_sCheData.sJgData.wType == 0x11);
			m_sCheData.sJgData.wItemCnt = *(WORD*)&pBuf[2];
			memcpy(m_sCheData.sJgData.unKownBytes_06, &pBuf[4], 0x06);

			if (m_sCheData.sJgData.wItemCnt > 0)
			{
				fread(m_sCheData.sJgData.unKownBytes_08, 1, 0x08, fp);	
				
				USES_CONVERSION;
				for (int i = 0; i < m_sCheData.sJgData.wItemCnt; i++)
				{
					sJfItem3 sItem;
					WORD wNameCnt = 0;
					fread(&wNameCnt, 1, 2, fp);	//长度

					char sName[200] = {0};
					fread(sName, 1, wNameCnt, fp);
					sName[wNameCnt] = 0;

					sItem.sGroupName = A2T(sName);

					char sTmp[0x3C] = {0};
					fread(sTmp, 1, 0x3C, fp);

					sItem.fLiveTime = *(float*)&sTmp[0x08];		//保留时间

					sItem.fContentsVal = *(float*)&sTmp[0x0C];	

					sItem.fLivePower = *(float*)&sTmp[0x1C];	//保留时间对应的电压
					sItem.nIdxMatch = *(int*)&sTmp[0x28];	//	
					sItem.nIdx = *(int*)&sTmp[0x2C];	//

					sItem.fTopSqrt = *(float*)&sTmp[0x00];
					sItem.fTopHVal = *(float*)&sTmp[0x04];
					sItem.fUnKonw3 = *(float*)&sTmp[0x10];
					sItem.fUnKonw4 = *(float*)&sTmp[0x14];
					sItem.fUnKonw5 = *(float*)&sTmp[0x18];
					sItem.fTopWVal = *(float*)&sTmp[0x20];
					sItem.fUnKonw7 = *(float*)&sTmp[0x24];

					sItem.dwUnkown1= *(DWORD*)&sTmp[0x30];		//0
					sItem.dwUnkown2= *(DWORD*)&sTmp[0x34];		//-1
					assert(sItem.dwUnkown1 == 0);
					assert(sItem.dwUnkown2 == 0xffffffff);

					sItem.fUnKonw8 = *(float*)&sTmp[0x38];
					m_sCheData.sJgData.verItems.push_back(sItem);
				}
				int nreal = fread(m_sCheData.unKownBytes_5e, 1, 0x5e, fp);
				assert(nreal == 0x5e);
			}		
		}

		m_sCheData.nEndByesCnt = fread(m_sCheData.unKownBytes_end, 1, 0x100, fp);
		assert(m_sCheData.nEndByesCnt > 0 && m_sCheData.nEndByesCnt < 0x100);

	} while (false);



	fclose(fp);


	m_strInputfile = sInput;

	return true;
}


void CCheFileData::ChangeDataRange(int nFrom, int nTo)
{
	m_sCheData.nDataCnt = nTo - nFrom;
	m_nSaveDataFromIdx = nFrom;
}

void CCheFileData::ChangeDataCnt(int nCnt)
{
	m_sCheData.nDataCnt = nCnt;
}

int	CCheFileData::GetDataCnt()
{
	assert(m_sCheData.nDataCnt == m_sCheData.verMainDatas.size());
	return m_sCheData.nDataCnt;
}

long CCheFileData::DataToValue(long ldt)
{
	DWORD dwData = ldt;
	dwData -= 0x800000;
	double fData = (int)dwData;
	fData /= 6.553599834442139;
	return (long)fData;
}

long CCheFileData::ValueToData(long lvar)
{
	double newVal = lvar * 6.553599834442139;
	int nVal = (int)newVal;
	nVal += 0x800000;
	return (long)nVal;
}

bool CCheFileData::GetDataByIdx(int nIdx, double & fRetData)
{
	if (nIdx < m_sCheData.verMainDatas.size())
	{
		DWORD dwData = m_sCheData.verMainDatas.at(nIdx);
		fRetData = DataToValue(dwData);
		return true;
	}
	return false;
}

bool CCheFileData::SetDataByIdx(int nIdx, double fVal)
{
	if (nIdx < m_sCheData.verMainDatas.size())
	{
		m_sCheData.verMainDatas[nIdx] =  ValueToData((long)fVal);
		return true;
	}
	return false;
}

int	CCheFileData::TimeToIdx(double dVal)
{
	dVal *= 60000.0f;
	dVal  /= 50;
	return (int)dVal;
}

double CCheFileData::IdxToTime(int nIdx)
{
	double dval = nIdx * 50;
	dval /= 60000.0;
	return dval;
}

double CCheFileData::GetXValue(int nIdx)
{
	return IdxToTime(nIdx);
}

int	 CCheFileData::GetWaveCnt()
{
	return m_sCheData.sJfData2.verItems.size();
}

bool CCheFileData::GetWaveByIdx(int nIdx, sJfItem & sItem)
{
	if (nIdx >= GetWaveCnt())
	{
		return false;
	}
	sItem = m_sCheData.sJfData.verItems.at(nIdx);
	return true;
}

void CCheFileData::ZoomWave(int nIdx, float fZoom)
{
	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nIdx);

	int nIdxFrom = sItem.nBeginDataIdx;
	int nIdxTo = sItem.nEndDataIdx;
	for (int i = nIdxFrom ; i <= nIdxTo; i++)
	{
		double dVal = 0;
		if (GetDataByIdx(i, dVal))
		{
			dVal *= (fZoom - fZoom / 2);
			SetDataByIdx(i, dVal);
		}
	}
}

bool CCheFileData::GetWaveTopByIdx(int nIdx, int & nTop)
{
	if (nIdx >= GetWaveCnt())
	{
		return false;
	}
	nTop = m_sCheData.sJfData2.verItems[nIdx].nTopHVal;
	return true;
}

bool CCheFileData::GetWaveLiveTimeByIdx(int nIdx, double & fTimeLive)
{
	if (nIdx >= GetWaveCnt())
	{
		return false;
	}

	sJfItem3 & item = m_sCheData.sJgData.verItems[nIdx];
	assert(nIdx == item.nIdx);
	fTimeLive = item.fLiveTime;
	return true;
}

#define PI 3.1415926
void GetWavePoints(int nHeight, int nFrom, int nTo, long * aPoint)
{
	int nWidth = nTo - nFrom;
	for(int i=0;i<nWidth;i++)
	{
		long lv = nHeight - (int)((nHeight)*(1-(sin(2*PI*i/nWidth / 2))));
		TRACE(L"-----%d\n", lv);
		aPoint[i] = CCheFileData::ValueToData(lv);
	}
}

void CCheFileData::NormalizeWave(int nWaveIdx)
{
	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nWaveIdx);
	int nIdxFrom = sItem.nBeginDataIdx;
	int nIdxTo = sItem.nEndDataIdx;

	double dv1, dv2;
	GetDataByIdx(nIdxFrom - 1, dv1);
	GetDataByIdx(nIdxTo + 1, dv2);
	for (int i = nIdxFrom; i <= nIdxTo; i++)
	{
		SetDataByIdx(i, GetRandomVal(dv1, dv2, i - nIdxFrom, nIdxTo - nIdxFrom));
	}
}

bool CCheFileData::ResetStdWave(int nWaveIdx, int nFromIdx, int nToIdx, long lTop)
{
	if (nWaveIdx >= GetWaveCnt() || lTop  < 1)
	{
		return false;
	}
	
	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nWaveIdx);
	int nOldTop = sItem.nTopHVal;
	int nIdxFrom = sItem.nBeginDataIdx;
	int nIdxTo = sItem.nEndDataIdx;
	NormalizeWave(nWaveIdx);

	if (nFromIdx < 0)
	{
		nFromIdx = nIdxFrom;
	}
	if (nToIdx < 0)
	{
		nToIdx = nIdxTo;
	}
	GetWavePoints(lTop, nIdxFrom, nIdxTo, (long*)(m_sCheData.verMainDatas.data() + nIdxFrom));
	return true;
}

bool CCheFileData::ChangeWaveSqrt(int nIdx, int nSqrt)
{
	if (nIdx >= GetWaveCnt() || nSqrt  < 1)
	{
		return false;
	}

	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nIdx);
	int nOld = sItem.nTopSqrt;
	sItem.nTopSqrt = nSqrt;

	m_sCheData.fTopSqrtTotal += (nSqrt - nOld);

	return true;
}

bool CCheFileData::ChangeWaveTop(int nIdx, int nTop)
{
	if (nIdx >= GetWaveCnt() || nTop  < 1)
	{
		return false;
	}
	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nIdx);
	int nleftH = sItem.nTopHVal - sItem.nTopHFrom;
	int nrightH = sItem.nTopHVal - sItem.nTopHTo;

	double fZoom = (double)nTop / sItem.nTopHVal;

	int nleftNewH = nTop - sItem.nTopHFrom;
	int nrightNewH = nTop - sItem.nTopHTo;

	int nIdxFrom = sItem.nBeginDataIdx;
	int nIdxTo = sItem.nTopDataIdx;
	vector<double> arrDataZoom;
	for (int i = nIdxFrom ; i <= nIdxTo; i++)
	{
		double dVal = 0;
		if (GetDataByIdx(i, dVal))
		{
			double fZoomV = (dVal - sItem.nTopHFrom) / nleftH;
			double dNew = nleftNewH * fZoomV + sItem.nTopHFrom;
			SetDataByIdx(i, dNew);
		}
	}

	nIdxFrom = sItem.nTopDataIdx + 1;
	nIdxTo = sItem.nEndDataIdx;
	for (int i = nIdxFrom ; i <= nIdxTo; i++)
	{
		double dVal = 0;
		if (GetDataByIdx(i, dVal))
		{
			double fZoomV = (dVal - sItem.nTopHTo) / nrightH;
			double dNew = nrightNewH * fZoomV + sItem.nTopHTo;
			SetDataByIdx(i, dNew);
		}
	}

	sItem.nTopHVal = nTop;
	sItem.nTopHPos *= fZoom;

	return true;
}

void CCheFileData::ChangeTimes(DATE dtFrom)
{
	COleDateTime dt = dtFrom;
	CString s = dt.Format(L"%Y-%m_%d %H-%M-%S");
	double ddiff = m_sCheData.dtOle2 - m_sCheData.dtOle1;
	m_sCheData.dtOle1 = dtFrom;
	if (m_sCheData.dtOle2)
	{
		m_sCheData.dtOle2 += ddiff;
	}
	if (m_sCheData.dtOle3)
	{
		m_sCheData.dtOle3 += ddiff;
	}
	if (m_sCheData.dtOle4)
	{
		m_sCheData.dtOle4 += ddiff;
	}
}

int	CCheFileData::TestTimeRange(double tFrom, double tEnd, int butIdx)
{
	int nWaveCnt = GetWaveCnt();
	int nRangData = GetDataCnt();
	if (TimeToIdx(tEnd) > nRangData - 1)
	{
		m_lastErr = L"超越最大值";
		return -2;
	}

	if (tFrom < 0)
	{
		m_lastErr = L"低于最小值";
		return -2;
	}

	int nConflictIdx = -1;
	for (int i = 0; i < nWaveCnt; i++)
	{
		if (i == butIdx)
		{
			continue;
		}
		sJfItem sItem;
		GetWaveByIdx(i, sItem);
		if (tFrom >= sItem.fTimeFrom && tFrom <= sItem.fTimeTo)
		{
			nConflictIdx = i;
			break;
		}
		if (tEnd >= sItem.fTimeFrom && tEnd <= sItem.fTimeTo)
		{
			nConflictIdx = i;
			break;
		}
	}
	return nConflictIdx;
}

void GetNewTimes(double f1, double f2, double fMid, double fNew, double & f3, double & f4)
{
	double fOld = f2 - f1;
	double fZoom = fNew / fOld;
	double fz1 = (fMid - f1) / fOld;
	double fz2 = (f2 - fMid) / fOld;
	double fDiff = abs(fNew - fOld);

	if (fZoom > 1)
	{
		if (fz1 > fz2)
		{
			double dav = (fMid - f1) - (f2 - fMid);
			double fd = dav - fDiff;
			if (fd >= 0)
			{
				f3 = f1;
				f4 = f2 + fDiff;
			}
			else
			{
				f3 = fMid - fNew / 2;
				f4 = fMid + fNew / 2;
			}
		}
		else
		{
			double dav = (f2 - fMid) - (fMid - f1);
			double fd = dav - fDiff;
			if (fd >= 0)
			{
				f3 = f1 - fDiff;
				f4 = f2;
			}
			else
			{
				f3 = fMid - fNew / 2;
				f4 = fMid + fNew / 2;
			}
		}
	}
	else
	{
		if (fz1 > fz2)
		{
			double dav = (fMid - f1) - (f2 - fMid);
			double fd = dav - fDiff;
			if (fd >= 0)
			{
				f3 = f1 + fDiff;
				f4 = f2;
			}
			else
			{
				f3 = fMid - fNew / 2;
				f4 = fMid + fNew / 2;
			}
		}
		else
		{
			double dav = (f2 - fMid) - (fMid - f1);
			double fd = dav - fDiff;
			if (fd >= 0)
			{
				f3 = f1;
				f4 = f2 - fDiff;
			}
			else
			{
				f3 = fMid - fNew / 2;
				f4 = fMid + fNew / 2;
			}
		}
	}
}

bool CCheFileData::ChangeWaveTimeWidth(int nIdx, double tWidth)
{
	int nWaveCnt = GetWaveCnt();
	if (nIdx >= nWaveCnt)
	{
		return false;
	}
	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nIdx);
	double dNewFromTime = 0;
	double dNewToTime = 0;
	GetNewTimes(IdxToTime(sItem.nBeginDataIdx), IdxToTime(sItem.nEndDataIdx), IdxToTime(sItem.nTopDataIdx), tWidth, dNewFromTime, dNewToTime);


	int nCid = TestTimeRange(dNewFromTime, dNewToTime, nIdx);
	if (nCid > -1)
	{
		sJfItem2 & sItem2 = m_sCheData.sJfData2.verItems.at(nCid);
		if (nIdx > nCid)
		{			
			double fDiff = IdxToTime(sItem2.nEndDataIdx) - dNewFromTime;
			dNewFromTime += fDiff;
			dNewToTime += fDiff;
		}
		else
		{
			double fDiff = dNewToTime - IdxToTime(sItem2.nBeginDataIdx);
			dNewFromTime -= fDiff;
			dNewToTime -= fDiff;
		}

		nCid = TestTimeRange(dNewFromTime, dNewToTime, nIdx);
		if (nCid != -1)
		{
			m_lastErr = L"无法设置新的峰宽，峰宽过大，与其他峰冲突！";
			return false;
		}
	}
	else if (nCid == -2)
	{
		double fDiff = dNewToTime - IdxToTime(m_sCheData.nDataCnt);
		dNewFromTime -= fDiff;
		dNewToTime -= fDiff;
		nCid = TestTimeRange(dNewFromTime, dNewToTime, nIdx);
		if (nCid > -1)
		{
			m_lastErr = L"无法设置新的峰宽，峰宽过大，超出边界值！";
			return false;
		}
	}

	if(ChangeWaveTimeRange(nIdx, dNewFromTime, dNewToTime))
	{
		sItem.nBeginDataIdx = TimeToIdx(dNewFromTime);
		sItem.nEndDataIdx = TimeToIdx(dNewToTime);

		auto & si = m_sCheData.sJfData.verItems.at(nIdx);
		si.fTimeFrom = dNewFromTime;
		si.fTimeTo = dNewToTime;
		return true;
	}
	return false;
}


void PushVal(int nv1, int nv2, int nIdx, int nRange, vector<int> & arrOutData)
{
	double dcnt = nv2 - nv1;
	double dNode = dcnt / nRange;
	double dv = nv1 + nIdx * dNode;
	arrOutData.push_back((int)dv);
}

void _ZoomTimeData(CCheFileData * pData, int nIdxFrom, int nIdxTo, int nNewCnt, vector<int> & arrOutData)
{
	int nCnt = nIdxTo - nIdxFrom;
	int nc = nNewCnt / nCnt;
	int noffset = nNewCnt % nCnt;
	double nstep = noffset > 0 ? (nCnt - 1) / (double)noffset : 0;
	double dcnt = 0;
	int nvi = 0;
	int nLastVal = 0;
	double nfCnt = 0;
	double dItem;
	if(pData->GetDataByIdx(nIdxFrom, dItem))
	{
		nLastVal = dItem;
	}
	for(int i = nIdxFrom + 1; i <= nIdxTo; i++)
	{
		int ncc = nc;
		int nss = nstep;// - (int)nstep > 0 ? (int)nstep + 1 : (int)nstep;
		nfCnt += 1;
		if (nstep > 0 && nfCnt > nstep)
		{
			nfCnt -= nstep; 
			ncc++;
		}
		
		int pi = 1;
		int nv = 0;
		int _lastVal = nLastVal;
		for (int j = 0; j < nc; j++)
		{
			if(pData->GetDataByIdx(i, dItem))
			{
				nv = dItem;
				PushVal(_lastVal, nv, pi++, ncc, arrOutData);
				nLastVal = nv;
			}
		}

		if (ncc > nc)
		{
			if (nc == 0)
			{
				if(pData->GetDataByIdx(i, dItem))
				{
					nv = dItem;
				}
			}
			PushVal(_lastVal, nv, pi, ncc, arrOutData);
			nLastVal = nv;
		}	
	}

	assert(nNewCnt == arrOutData.size());
}

void ZoomTimeData(CCheFileData * pData, int nIdxFrom, int nIdxTo, int nNewCnt, vector<int> & arrOutData)
{
	int nCnt = nIdxTo - nIdxFrom;
	int nc = nNewCnt / nCnt;
	int noffset = nNewCnt % nCnt;
	double nstep = noffset > 0 ? (nCnt - 1) / (double)noffset : 0;
	double dcnt = 0;
	int nvi = 0;
	int nLastVal = 0;
	double dItem;
	if(pData->GetDataByIdx(nIdxFrom, dItem))
	{
		nLastVal = dItem;
	}
	for(int i = nIdxFrom + 1; i <= nIdxTo; i++)
	{
		int nss = nstep;// - (int)nstep > 0 ? (int)nstep + 1 : (int)nstep;
		int pi = 0;
		int nv = 0;
		int ncc = nc;
		dcnt += (nstep - (int)nstep);

		if (dcnt > nstep)
		{
			//dcnt -= (int)nstep;
			nss++;
		}
		else
		if (i == nIdxTo && dcnt > 0)
		{
			nc++;
			ncc = nc;
		}

		if((int)nstep > 0 && i % (int)nss == 0)
		{
			//dcnt -= (int)nstep;
			ncc ++;
		}
		else
		{
			if (dcnt > nstep)
			{
				dcnt -= (int)nstep;
				//ncc++;
			}
		}
		
		for (int j = 0; j < nc; j++)
		{
			if(pData->GetDataByIdx(i, dItem))
			{
				nv = dItem;
				PushVal(nLastVal, nv, pi++, ncc, arrOutData);
				nLastVal = nv;
			}
		}
	
		if (ncc > nc)
		{
			if (nc == 0)
			{
				if(pData->GetDataByIdx(i, dItem))
				{
					nv = dItem;
				}
			}
			PushVal(nLastVal, nv, pi, ncc, arrOutData);
			nLastVal = nv;
		}	
	}

	assert(nNewCnt == arrOutData.size());
}

bool CCheFileData::ChangeWaveTimeRange(int nIdx, double tFrom, double tEnd)
{
	int nWaveCnt = GetWaveCnt();
	if (nIdx >= nWaveCnt)
	{
		return false;
	}

	int nConflictIdx = TestTimeRange(tFrom, tEnd, nIdx);
	if (nConflictIdx >= 0 || nConflictIdx < -1)
	{
		return false;
	}

	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nIdx);
	int nOldTop = sItem.nTopHVal;
	int nIdxFrom = sItem.nBeginDataIdx;
	int nIdxTo = sItem.nEndDataIdx;
	int nIdxTop = sItem.nTopDataIdx;

	int nIdxNewFrom = TimeToIdx(tFrom);
	int nIdxNewTo = TimeToIdx(tEnd);

	if (nIdxNewFrom >= nIdxTop)
	{
		return false;
	}
	if (nIdxNewTo <= nIdxTop)
	{
		return false;
	}

	vector<int> arrLeftData;
	_ZoomTimeData(this, nIdxFrom, nIdxTop, nIdxTop - nIdxNewFrom, arrLeftData);
	vector<int> arrRightData;
	_ZoomTimeData(this, nIdxTop, nIdxTo, nIdxNewTo - nIdxTop, arrRightData);

	NormalizeWave(nIdx);
	
	for (int i = 0; i < arrLeftData.size(); i++)
	{
		SetDataByIdx(i + nIdxNewFrom, arrLeftData[i]);
	}
	for (int i = 0; i < arrRightData.size(); i++)
	{
		SetDataByIdx(i + nIdxTop, arrRightData[i]);
	}

	return true;
}

int	CCheFileData::GetRandomVal(int nFrom, int nTo, int nIdx, int nTimeRange)
{
	int nRange = nTo - nFrom + 1;
	float fnode = nRange / nTimeRange;
	double rval = (int)rand() % (int)10 - 5;
	nFrom += (nIdx * fnode  + rval);
	return nFrom;
}

bool CCheFileData::ChangeWaveTimePos(int nIdx, double tLive )
{
	if (nIdx >= GetWaveCnt())
	{
		m_lastErr = L"超出数据范围";
		return false;
	}

	double fOldLive = 0;
	if(!GetWaveLiveTimeByIdx(nIdx, fOldLive))
	{	
		m_lastErr = L"获取保留时间失败";
		return false;
	}

	if ((float)tLive == (float)fOldLive)
	{
		m_lastErr = L"";
		return false;
	}

	double fOffset = tLive - fOldLive;
	sJfItem sItem;
	GetWaveByIdx(nIdx, sItem);

	int nIdxFromOld = TimeToIdx(sItem.fTimeFrom);
	int nIdxToOld = TimeToIdx(sItem.fTimeTo);

	double tFrom = sItem.fTimeFrom + fOffset;
	double tEnd = sItem.fTimeTo + fOffset;

	int nConflictIdx = TestTimeRange(tFrom, tEnd, nIdx);
	if (nConflictIdx >= 0)
	{
		m_lastErr = L"波形重合，数据文件不支持此类型";
		return false;
	}
	if(nConflictIdx == -2)
	{
		m_lastErr = L"保留时间超出最大值，不允许";
		return false;
	}

	int nIdxFrom = TimeToIdx(tFrom);
	int nIdxTo = nIdxFrom + (nIdxToOld - nIdxFromOld);
	int * pDataBak = new int[nIdxToOld - nIdxFromOld + 1];
	for (int i = nIdxFromOld; i <= nIdxToOld; i++)
	{
		double dv = 0;
		GetDataByIdx(i, dv);
		pDataBak[i - nIdxFromOld] = dv;
	}

	NormalizeWave(nIdx);

	for (int i = nIdxFrom; i <= nIdxTo; i++)
	{
		SetDataByIdx(i, pDataBak[i - nIdxFrom]);
	}

	delete [] pDataBak;

	sJfItem3 & item = m_sCheData.sJgData.verItems[nIdx];
	item.fLiveTime = (float)tLive;

	m_sCheData.sJfData.verItems[nIdx].fTimeFrom = tFrom;
	m_sCheData.sJfData.verItems[nIdx].fTimeTo = tEnd;

	sJfItem2 & item2 = m_sCheData.sJfData2.verItems[nIdx];
	item2.nBeginDataIdx = nIdxFrom;
	item2.nTopDataIdx = TimeToIdx(tLive);
	item2.nEndDataIdx = nIdxTo;

	return true;
}

void CCheFileData::Clear()
{
	srand(time(NULL));
	m_nSaveDataFromIdx = 0;

	memset(&m_sCheData.sVer[0], 0, 0x22);
	m_sCheData.nDataCnt = 0;
	m_sCheData.nYLimit_High  = 0;
	m_sCheData.nYLimit_Low = 0;
	m_sCheData.verMainDatas.clear();
	memset(&m_sCheData.unKownBytes_0a[0], 0, 0x0a);
	memset(&m_sCheData.unKownBytes_2e[0], 0, 0x2E);
	m_sCheData.dtOle1 = 0;
	m_sCheData.dtOle2 = 0;
	m_sCheData.nUnKnowVal2 = 0;
	m_sCheData.dtOle3 = 0;
	m_sCheData.dtOle4 = 0; 

	m_sCheData.sJfData.nItemCnt = 0;
	m_sCheData.sJfData.wType = 0;
	m_sCheData.sJfData.wItemCnt = 0;
	m_sCheData.sJfData.verItems.clear();

	m_sCheData.sJfData2.nItemCnt = 0;
	m_sCheData.sJfData2.wType = 0;
	m_sCheData.sJfData2.wItemCnt = 0;
	m_sCheData.sJfData2.verItems.clear();
	
	m_sCheData.sJgData.wType = 0;
	m_sCheData.sJgData.wItemCnt = 0;
	memset(&m_sCheData.sJgData.unKownBytes_06[0], 0, 6);
	memset(&m_sCheData.sJgData.unKownBytes_08[0], 0, 8);

	memset(&m_sCheData.unKownBytes_5e[0], 0, 0x5e);
	memset(&m_sCheData.unKownBytes_end[0], 0, 0x100);
	m_sCheData.nEndByesCnt = 0;
	m_sCheData.sJgData.verItems.clear();

	m_sCheData.fTopSqrtTotal = 0;
}
