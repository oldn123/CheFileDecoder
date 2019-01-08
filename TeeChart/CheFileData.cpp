#include "StdAfx.h"
#include "CheFileData.h"
#include <assert.h>

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
		
		//assert(*(int*)&m_sCheData.unKownBytes_2e[6] == m_sCheData.nDataCnt);
		*(int*)&m_sCheData.unKownBytes_2e[6] = m_sCheData.nDataCnt;
		fwrite(m_sCheData.unKownBytes_2e, 1, 0x2E, fp);

		{
			char sBuf[0x2C] = {0};
			assert(m_sCheData.sJfData.wType == 3);
			assert(m_sCheData.sJfData.nItemCnt == m_sCheData.sJfData.wItemCnt);	

			*(DATE*)&sBuf[0] = (DATE)m_sCheData.dtOle1;
			*(DATE*)&sBuf[8] = (DATE)m_sCheData.dtOle2;
			*(int*)&sBuf[0x10] = m_sCheData.nUnKnowVal2;
			*(DATE*)&sBuf[0x14] = (DATE)m_sCheData.dtOle3;
			*(DATE*)&sBuf[0x1C] = (DATE)m_sCheData.dtOle4;

			*(WORD*)&sBuf[0x24] = m_sCheData.sJfData.wType;
			*(WORD*)&sBuf[0x26] = m_sCheData.sJfData.wItemCnt;
			*(WORD*)&sBuf[0x28] = m_sCheData.sJfData.nItemCnt;
			fwrite(sBuf, 1, 0x2C, fp);
		}

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
			*(WORD*)&pJfDatas2[4] = m_sCheData.sJfData2.nItemCnt;
			assert(m_sCheData.sJfData2.nItemCnt == m_sCheData.sJfData2.verItems.size());

			for(int i = 0; i < m_sCheData.sJfData2.nItemCnt; i++)
			{
				int nItemFrom = 8 + i * 0x2C;
				sJfItem2 & si = m_sCheData.sJfData2.verItems[i];

				*(WORD*)(&pJfDatas2[nItemFrom] + 0) = si.wUnKnow1;
				*(DWORD*)(&pJfDatas2[nItemFrom] + 2)= si.dwUnknow1;
				*(DWORD*)(&pJfDatas2[nItemFrom] + 6)= si.dwUnknow2;
				*(DWORD*)(&pJfDatas2[nItemFrom] + 0x0A)	= si.dwUnknow3;
				*(DWORD*)(&pJfDatas2[nItemFrom] + 0x0E)	= si.dwUnknow4;
				*(DWORD*)(&pJfDatas2[nItemFrom] + 0x12)	= si.nTopHPos;
				*(DWORD*)(&pJfDatas2[nItemFrom] + 0x16)	= si.dwUnknow6;
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

		{
			char buf[0x12] = {0};
			char * pBuf = &buf[0];

			*(WORD*)&pBuf[0] = m_sCheData.sJgData.wType;
			*(WORD*)&pBuf[2] = m_sCheData.sJgData.wItemCnt;
			*(WORD*)&pBuf[4] = m_sCheData.sJgData.nItemCnt;
			memcpy(&pBuf[8], m_sCheData.sJgData.unKownBytes_0a, 0x0a);
			fwrite(pBuf, 1, 0x12, fp);	//总量

			assert(m_sCheData.sJgData.nItemCnt == m_sCheData.sJgData.verItems.size());
			USES_CONVERSION;
			for (int i = 0; i < m_sCheData.sJgData.nItemCnt; i++)
			{
				sJfItem3 & sItem = m_sCheData.sJgData.verItems.at(i);
				WORD wNameCnt = ((CStringA)sItem.sGroupName).GetLength();
				fwrite(&wNameCnt, 1, 2, fp);	//总量

				fwrite(((CStringA)sItem.sGroupName).GetBuffer(), 1, wNameCnt, fp);

				char sTmp[0x3C] = {0};
				
				*(float*)&sTmp[0x08] = sItem.fLiveTime;		//保留时间

				*(float*)&sTmp[0x0C] = sItem.fContentsVal;	

				*(float*)&sTmp[0x1C] = sItem.fLivePower;	//保留时间对应的电压
				*(int*)&sTmp[0x28] = sItem.nIdx1;	//	
				*(int*)&sTmp[0x2C] = sItem.nIdx2;	//

				assert(sItem.nIdx1 == sItem.nIdx2);

				*(float*)&sTmp[0x00] = sItem.fTopSqrt;
				*(float*)&sTmp[0x04] = sItem.fTopHVal;
				*(float*)&sTmp[0x10] = sItem.fUnKonw3;
				*(float*)&sTmp[0x14] = sItem.fUnKonw4;
				*(float*)&sTmp[0x18] = sItem.fUnKonw5;
				*(float*)&sTmp[0x20] = sItem.fUnKonw6;
				*(float*)&sTmp[0x24] = sItem.fUnKonw7;
									
				*(DWORD*)&sTmp[0x30] = sItem.dwUnkown1;		//0
				*(DWORD*)&sTmp[0x34] = sItem.dwUnkown2;		//-1
				assert(sItem.dwUnkown1 == 0);
				assert(sItem.dwUnkown2 == 0xffffffff);

				*(float*)&sTmp[0x38] = sItem.fUnKonw8;
				
				fwrite(sTmp, 1, 0x3C, fp);
			}
		}
		
		fwrite(m_sCheData.unKownBytes_aa, 1, 0xaa, fp);

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
			m_sCheData.sJfData.nItemCnt = *(WORD*)&sBuf[0x28];
		}

		assert(m_sCheData.sJfData.wType == 3);
		assert(m_sCheData.sJfData.nItemCnt == m_sCheData.sJfData.wItemCnt);

		{
			char * pJfDatas = new char[0x41 * 0x12];

			fread(pJfDatas, 0x12, 0x41, fp);	//总量
			
			for(int i = 0; i < m_sCheData.sJfData.nItemCnt; i++)
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
		}

		{
			char * pJfDatas2 = new char[0x41 * 0x2C + 8];

			fread(pJfDatas2, 1, 0x41 * 0x2C + 8, fp);	//总量

			m_sCheData.sJfData2.wType = *(WORD*)&pJfDatas2[0];
			m_sCheData.sJfData2.wItemCnt = *(WORD*)&pJfDatas2[2];
			m_sCheData.sJfData2.nItemCnt = *(WORD*)&pJfDatas2[4];
			m_sCheData.fTopSqrtTotal = 0;
			for(int i = 0; i < m_sCheData.sJfData2.nItemCnt; i++)
			{
				int nItemFrom = 8 + i * 0x2C;
				sJfItem2 si;
				si.wUnKnow1 = *(WORD*)(&pJfDatas2[nItemFrom] + 0);
				si.dwUnknow1= *(DWORD*)(&pJfDatas2[nItemFrom] + 2);
				si.dwUnknow2= *(DWORD*)(&pJfDatas2[nItemFrom] + 6);
				si.dwUnknow3= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x0A);
				si.dwUnknow4= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x0E);
				si.nTopHPos = *(DWORD*)(&pJfDatas2[nItemFrom] + 0x12);
				si.dwUnknow6= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x16);
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
			char buf[0x12] = {0};
			char * pBuf = &buf[0];

			fread(pBuf, 1, 0x12, fp);	//总量

			m_sCheData.sJgData.wType = *(WORD*)&pBuf[0];
			m_sCheData.sJgData.wItemCnt = *(WORD*)&pBuf[2];
			m_sCheData.sJgData.nItemCnt = *(WORD*)&pBuf[4];
			memcpy(m_sCheData.sJgData.unKownBytes_0a, &pBuf[8], 0x0a);

			USES_CONVERSION;
			for (int i = 0; i < m_sCheData.sJgData.nItemCnt; i++)
			{
				sJfItem3 sItem;
				WORD wNameCnt = 0;
				fread(&wNameCnt, 1, 2, fp);	//总量
				
				char sName[200] = {0};
				fread(sName, 1, wNameCnt, fp);
				sName[wNameCnt] = 0;

				sItem.sGroupName = A2T(sName);

				char sTmp[0x3C] = {0};
				fread(sTmp, 1, 0x3C, fp);

				sItem.fLiveTime = *(float*)&sTmp[0x08];		//保留时间
				
				sItem.fContentsVal = *(float*)&sTmp[0x0C];	

				sItem.fLivePower = *(float*)&sTmp[0x1C];	//保留时间对应的电压
				sItem.nIdx1 = *(int*)&sTmp[0x28];	//	
				sItem.nIdx2 = *(int*)&sTmp[0x2C];	//

				assert(sItem.nIdx1 == sItem.nIdx2);

				sItem.fTopSqrt = *(float*)&sTmp[0x00];
				sItem.fTopHVal = *(float*)&sTmp[0x04];
				sItem.fUnKonw3 = *(float*)&sTmp[0x10];
				sItem.fUnKonw4 = *(float*)&sTmp[0x14];
				sItem.fUnKonw5 = *(float*)&sTmp[0x18];
				sItem.fUnKonw6 = *(float*)&sTmp[0x20];
				sItem.fUnKonw7 = *(float*)&sTmp[0x24];

				sItem.dwUnkown1= *(DWORD*)&sTmp[0x30];		//0
				sItem.dwUnkown2= *(DWORD*)&sTmp[0x34];		//-1
				assert(sItem.dwUnkown1 == 0);
				assert(sItem.dwUnkown2 == 0xffffffff);

				sItem.fUnKonw8 = *(float*)&sTmp[0x38];
				m_sCheData.sJgData.verItems.push_back(sItem);
			}
		}

		int nreal = fread(m_sCheData.unKownBytes_aa, 1, 0xaa, fp);	//总量
		assert(nreal == 0xaa);

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

bool CCheFileData::GetDataByIdx(int nIdx, double & fRetData)
{
	if (nIdx < m_sCheData.verMainDatas.size())
	{
		DWORD dwData = m_sCheData.verMainDatas.at(nIdx);
		dwData -= 0x800000;
		double fData = (int)dwData;
		fData /= 6.553599834442139;
		fRetData = fData;
		return true;
	}
	return false;
}

bool CCheFileData::SetDataByIdx(int nIdx, double fVal)
{
	if (nIdx < m_sCheData.verMainDatas.size())
	{
		double newVal = fVal * 6.553599834442139;
		int nVal = (int)newVal;
		nVal += 0x800000;
		m_sCheData.verMainDatas[nIdx] = (DWORD)nVal;
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
	return m_sCheData.sJfData.verItems.size();
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

void CCheFileData::ZoomWave(int nIdx, sJfItem & item, float fZoom)
{
	int nIdxFrom = TimeToIdx(item.fTimeFrom);
	int nIdxTo = TimeToIdx(item.fTimeTo);
	for (int i = nIdxFrom ; i <= nIdxTo; i++)
	{
		double dVal = 0;
		if (GetDataByIdx(i, dVal))
		{
			dVal *= fZoom;
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
	assert(nIdx == item.nIdx1);
	fTimeLive = item.fLiveTime;
	return true;
}

bool CCheFileData::ChangeWaveTop(int nIdx, int nTop)
{
	if (nIdx >= GetWaveCnt() || nTop  < 1)
	{
		return false;
	}
	sJfItem sOld = m_sCheData.sJfData.verItems[nIdx];
		
	double fZoom = (double)nTop / m_sCheData.sJfData2.verItems[nIdx].nTopHVal;

	m_sCheData.sJfData2.verItems[nIdx].nTopHVal = nTop;

	m_sCheData.sJfData2.verItems[nIdx].nTopHPos *= fZoom;

	if (fZoom != 1)
	{
		ZoomWave(nIdx, sOld, fZoom);
	}
	return true;
}

void CCheFileData::ChangeTimes(DATE dtFrom)
{
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

bool CCheFileData::ChangeWaveTimeRange(int nIdx, double tFrom, double tEnd)
{
	int nWaveCnt = GetWaveCnt();
	if (nIdx >= nWaveCnt)
	{
		return false;
	}

	int nConflictIdx = TestTimeRange(tFrom, tEnd, nIdx);
	if (nConflictIdx >= 0)
	{
		return false;
	}

	return true;
}

int	CCheFileData::GetRandomVal(int nBaseVal)
{
	double rval = rand() % 100 / (float)100;
	nBaseVal += rval * 10;
	return nBaseVal;
}

bool CCheFileData::ChangeWaveTimePos(int nIdx, double tLive )
{
	if (nIdx >= GetWaveCnt())
	{
		return false;
	}

	double fOldLive = 0;
	if(!GetWaveLiveTimeByIdx(nIdx, fOldLive))
	{
		return false;
	}

	if ((float)tLive == (float)fOldLive)
	{
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
		return false;
	}

	int nIdxFrom = TimeToIdx(tFrom);
	int nIdxTo = nIdxFrom + (nIdxToOld - nIdxFromOld);
	DWORD * pDataBak = new DWORD[nIdxToOld - nIdxFromOld + 1];
	for (int i = nIdxFromOld; i <= nIdxToOld; i++)
	{
		pDataBak[i - nIdxFromOld] = m_sCheData.verMainDatas.at(i);
		m_sCheData.verMainDatas[i] = GetRandomVal(nIdxFromOld - 1);
	}

	for (int i = nIdxFrom; i <= nIdxTo; i++)
	{
		m_sCheData.verMainDatas[i] = pDataBak[i - nIdxFrom];
	}

	delete [] pDataBak;


	sJfItem3 & item = m_sCheData.sJgData.verItems[nIdx];
	item.fLiveTime = (float)tLive;

	m_sCheData.sJfData.verItems[nIdx].fTimeFrom = tFrom;
	m_sCheData.sJfData.verItems[nIdx].fTimeTo = tEnd;

	return true;
}

void CCheFileData::RemoveWave(int)
{

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

	m_sCheData.sJgData.nItemCnt = 0;
	m_sCheData.sJgData.wType = 0;
	m_sCheData.sJgData.wItemCnt = 0;
	memset(&m_sCheData.sJgData.unKownBytes_0a[0], 0, 0x0a);
	memset(&m_sCheData.unKownBytes_aa[0], 0, 0x0aa);
	m_sCheData.sJgData.verItems.clear();

	m_sCheData.fTopSqrtTotal = 0;
}
