#include "StdAfx.h"
#include "CheFileData.h"
#include <assert.h>
#include <math.h>
#include <map>

#define val_offset 2
#ifndef _DEBUG

//*****************
#define Use_dog
//*****************

#endif
#ifdef Use_dog
#include "..\..\dog\dog_api.h"
#include "..\..\dog\dog_api_cpp.h"
#include "..\..\dog\vendor_code.h"
#pragma comment(lib, "..\\..\\dog\\lib\\libdog_windows_3155421.lib")

dog_handle_t hdog = 0;
#endif

char g_sSign[10];
int	g_nNodeSize = 0;


void _ZoomTimeData(vector<int> & arrInData, int nNewCnt, vector<int> & arrOutData);

void CCheFileData::NormalizeWave(int nWaveIdx)
{
	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nWaveIdx);
	int nIdxFrom = sItem.nBeginDataIdx;
	int nIdxTo = sItem.nEndDataIdx;

	double dv1, dv2;
	GetDataByIdx(nIdxFrom - 1, dv1);
	GetDataByIdx(nIdxTo + 1, dv2);
	int nLen = nIdxTo - nIdxFrom + 1;

	vector<int> arrDatas;
	MakeLineVal(dv1, dv2, nLen, true, true, arrDatas);

	for (int i = nIdxFrom; i <= nIdxTo; i++)
	{
		SetDataByIdx(i, arrDatas[i - nIdxFrom]);
	}
}

void CCheFileData::FixWaveBottomLine(int nWaveIdx)
{
	sJfItem & sItem1 = m_sCheData.sJfData.verItems.at(nWaveIdx);
	sJfItem2 & sItem2 = m_sCheData.sJfData2.verItems.at(nWaveIdx);
	int nIdxFrom = sItem2.nBeginDataIdx;
	int nIdxTo = sItem2.nEndDataIdx;
	sItem1.fTimeFrom = IdxToTime(nIdxFrom);
	sItem1.fTimeTo = IdxToTime(nIdxTo);
	double d1, d2;
	GetDataByIdx(sItem2.nBeginDataIdx, d1);
	GetDataByIdx(sItem2.nEndDataIdx, d2);

	sItem2.nTopHFrom = d1;
	sItem2.nTopHTo = d2;

	sItem1.fPowerFrom = sItem2.nTopHFrom;
	sItem1.fPowerTo = sItem2.nTopHTo;
}


void CCheFileData::MakeLineVal(int nfrom, int nto, int ncnt, bool bHasLeft, bool bHasRigth, vector<int> & arr)
{
	arr.clear();
	if (ncnt < 1)
	{
		return;
	}
	if (ncnt > 1)
	{
		int nFromVal = 1;
		int nrc = ncnt;
		int nToVal = nrc;
		if (bHasLeft && bHasRigth)
		{
			nrc = ncnt - 1;
			nFromVal = 0;
			nToVal = nrc + 1;
		}
		else
		{
			if (bHasLeft)
			{
				nFromVal = 0;
			}
			else
				if (bHasRigth)
				{
					nToVal = nrc + 1;
				}
				else
				{
					nToVal = nrc + 1;
					nrc = ncnt + 1;
				}
		}
		double delta=(nto-nfrom)*1.0/nrc;   
		arr.reserve(ncnt);

		int nLast = 0;
		for (int i = nFromVal; i < nToVal; )
		{
			int nWStep = (int)rand() % m_nNorLineWidthStep + 1;
			if (nWStep + i > nToVal)
			{
				nWStep = nToVal - i;
			}

			double dv2 = (i - nFromVal + nWStep) * delta  + (int)rand() % (int)(2*m_nNorLineHeightStep) - m_nNorLineHeightStep;
			for (int j = 0; j < nWStep; j++)
			{
				double dv = nfrom + GetRandomVal(nLast, dv2, j, nWStep);
				arr.push_back((int)(dv+0.5));
			}
			i += nWStep;
			nLast = dv2;	
		}
	}
	else
	{
		arr.push_back(nfrom + (int)rand() % (int)(2*m_nNorLineHeightStep) - m_nNorLineHeightStep + (nto - nfrom) / 2);
	}

	assert(arr.size() == ncnt);
}


void CCheFileData::DoInit()
{

#ifdef _DEBUG
	strcpy(g_sSign, "CHERI10 ");
	g_nNodeSize = 0x2C;
#else
#ifdef Use_dog
	dog_status_t ret = dog_login(100, vendorCode, &hdog);
	if (ret != DOG_STATUS_OK)
	{
		MessageBox(NULL, L"未检测到加密狗！", L"错误", MB_ICONERROR);
		_exit(0);
	}
	memset(g_sSign, 0, 10);
	ret = dog_read(hdog, 1, 0, 8, g_sSign);
	ret = dog_read(hdog, 2, 0, 4, &g_nNodeSize);	
#else
	strcpy(g_sSign, "CHERI10 ");
	g_nNodeSize = 0x2C;
#endif

	bool btryok = true;
	bool btrymode = true;
	if (btrymode)
	{
		COleDateTime dtNow = COleDateTime::GetCurrentTime();
		do 
		{
			if (dtNow.GetYear() != 2019 )
			{
				btryok = false;
				break;
			}
			if (dtNow.GetMonth() != 3 )
			{
				btryok = false;
				break;
			}
			if (dtNow.GetDay() > 29 ||
				dtNow.GetDay() < 25)
			{
				btryok = false;
				break;
			}
		} while (false);
		if (!btryok)
		{
			_exit(0);
		}
	}
#endif

}

void CCheFileData::DoUninit()
{
#ifdef Use_Dog
	dog_logout(hdog);
#endif
}

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
			strcpy(sBuf, g_sSign);

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
					*(WORD*)(&pJfDatas2[nItemFrom] + 0x28)	= si.wUnKnow2;		// =0
					*(WORD*)(&pJfDatas2[nItemFrom] + 0x2A)	= si.wUnKnow3;		// =0
	
					assert(si.wUnKnow2 == 0);
					assert(si.wUnKnow3 == 0);
				}
				fwrite(pJfDatas2, 1, 0x41 * g_nNodeSize + 8, fp);	//总量
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

			if (strcmp(m_sCheData.sVer, g_sSign) != 0)
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
				si.wUnKnow2 = *(WORD*)(&pJfDatas2[nItemFrom] + 0x28);		// =0
				si.wUnKnow3 = *(WORD*)(&pJfDatas2[nItemFrom] + 0x2A);
				assert(si.wUnKnow2 == 0);
				assert(si.wUnKnow3 == 0);

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


	//修复item1
	if (m_sCheData.sJfData.verItems.size() != m_sCheData.sJfData2.verItems.size())
	{
		//重建item1
		m_sCheData.sJfData.verItems.clear();
		for (auto iter = m_sCheData.sJfData2.verItems.begin(); iter != m_sCheData.sJfData2.verItems.end(); iter++)
		{
			sJfItem item;
			item.fTimeFrom = IdxToTime(iter->nBeginDataIdx);
			item.fTimeTo = IdxToTime(iter->nEndDataIdx);
			item.fPowerFrom = iter->nTopHFrom;
			item.fPowerTo = iter->nTopHTo;
			item.wUnKnow = 0;
			m_sCheData.sJfData.verItems.push_back(item);
		}
		m_sCheData.sJfData.nItemCnt = m_sCheData.sJfData.verItems.size();
	}



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
	return (long)(fData+0.5f);
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

		//TRACE(L"GetDataByIdx: %d, %d\n", nIdx, (int)fRetData);
		return true;
	}
	return false;
}

bool CCheFileData::SetDataByIdx(int nIdx, double fVal)
{
	//TRACE(L"SetDataByIdx: %d, %d\n", nIdx, (int)fVal);
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
	return (int)(dVal + 0.5);
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

bool CCheFileData::GetWaveByIdx(int nIdx, sJfItem2 & sItem)
{
	if (nIdx >= GetWaveCnt())
	{
		return false;
	}
	sItem = m_sCheData.sJfData2.verItems.at(nIdx);
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

	sJfItem2 & item = m_sCheData.sJfData2.verItems[nIdx];
	fTimeLive = IdxToTime(item.nTopDataIdx);
	return true;
}

#define PI 3.1415926
void GetWavePoints(int nHeight, int nFrom, int nTo, long * aPoint)
{
	int nWidth = nTo - nFrom;
	for(int i=0;i<nWidth;i++)
	{
		long lv = nHeight - (int)((nHeight)*(1-(sin(2*PI*i/nWidth / 2))));
		//TRACE(L"-----%d\n", lv);
		aPoint[i] = CCheFileData::ValueToData(lv);
	}
}

void CCheFileData::FixWaveEdge(int nWaveIdx)
{
	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nWaveIdx);
	int nIdxFrom = sItem.nBeginDataIdx;
	int nIdxTo = sItem.nEndDataIdx;

	int nEndPos1 = 0;
	int nEndPos2 = m_sCheData.nDataCnt;
	int ndf1 = 0x7fffffff;
	int ndf2 = 0x7fffffff;
	for (int i = 0; i < GetWaveCnt(); i++)
	{
		if (i == nWaveIdx)
		{
			continue;
		}
		sJfItem2 tmp2;
		GetWaveByIdx(i, tmp2);
		int nl = nIdxFrom - tmp2.nEndDataIdx;
		if (nl > 0)
		{
			if (nl < ndf1)
			{
				ndf1 = nl;
				nEndPos1 = tmp2.nEndDataIdx + 1;
			}
		}
		int nl2 = tmp2.nBeginDataIdx - nIdxTo;
		if (nl2 > 0)
		{
			if (nl2 < ndf2)
			{
				ndf2 = nl2;
				nEndPos2 = tmp2.nBeginDataIdx - 1;
			}
		}
	}


	int nIdx = 0;
	double d1, d2;
	GetDataByIdx(nIdxFrom, d1);
	GetDataByIdx(nEndPos1, d2);

	vector<int> arrInput, arrOutput;
	arrInput.push_back(d1);
	arrInput.push_back(d2);

	_ZoomTimeData(arrInput, abs(nEndPos1 + 1 - nIdxFrom), arrOutput);
	for (int i = nIdxFrom; i > nEndPos1 + 1; i--)
	{
		SetDataByIdx(i - 1, arrOutput[nIdx++]);
	}


	arrInput.clear();
	arrOutput.clear();
	GetDataByIdx(nIdxTo, d1);
	GetDataByIdx(nEndPos2 - 1, d2);
	arrInput.push_back(d1);
	arrInput.push_back(d2);
	_ZoomTimeData(arrInput, abs(nEndPos2 - 1 - nIdxTo), arrOutput);
	nIdx = 0;
	for (int i = nIdxTo; i < nEndPos2 - 1; i++)
	{
		SetDataByIdx(i+1, arrOutput[nIdx++]);
	}
}

// 
// void CCheFileData::SmoothWave(int nWaveIdx)
// {
// 	sJfItem2 itemWave;
// 	if(!GetWaveByIdx(nWaveIdx, itemWave))
// 	{
// 		return ;
// 	}
// 
// 	int nFromIdx = itemWave.nBeginDataIdx;
// 	int nTopIdx = itemWave.nTopDataIdx;
// 	int nToIdx = itemWave.nEndDataIdx;
// 
// 	vector<double> diffArr;
// 	vector<double> srcArr;
// 	double nLastVal;
// 	GetDataByIdx(nFromIdx, nLastVal);
// 	int nBase = nFromIdx + 1;
// 	for (int i = nBase; i <= nTopIdx; i++)
// 	{
// 		double d1;
// 		GetDataByIdx(i, d1);
// 		srcArr.push_back(d1);
// 		diffArr.push_back(d1 - nLastVal);
// 		nLastVal = d1;
// 	}
// 
// 	int nSubRange = 9;
// 	int nStep = nSubRange / 2;
// 	vector<int > diffArr2;
// 	for (int i = nStep; i < diffArr.size() - nStep; i++)
// 	{
// 		int nTotal = 0;
// 		for (int j = i - nStep; j < (i - nStep) + nSubRange; j++)
// 		{
// 			nTotal += diffArr[j]; 
// 		}
// 		nTotal /= nSubRange;
// 		diffArr2.push_back(nTotal);
// 	}
// 
// 	int nIdx = 0;
// 	for (int i = nBase; i <= nTopIdx; i++)
// 	{
// 		if (i < nBase + nStep ||
// 			i > nTopIdx - nStep)
// 		{
// 			continue;
// 		}
// 		double d1 = srcArr[i - nBase];
// 		d1 += diffArr2[nIdx++];
// 		SetDataByIdx(i, d1);
// 	}
// }

void CCheFileData::SmoothWave(int nWaveIdx)
{
	sJfItem2 itemWave;
	if(!GetWaveByIdx(nWaveIdx, itemWave))
	{
		return ;
	}

	int nFromIdx = itemWave.nBeginDataIdx;
	int nTopIdx = itemWave.nTopDataIdx;
	int nToIdx = itemWave.nEndDataIdx;

	map<int, int>	keyPointMap;

	double dLast = -100000;
	for (int i = nFromIdx+1; i <= nTopIdx; i++)
	{	
		double d1;
		GetDataByIdx(i, d1);
		if ((d1 - dLast) > 2)
		{
			keyPointMap[i] = d1;
			dLast = d1;
		}
	}

	dLast = -100000;
	for (int i = nToIdx-1; i >= nTopIdx; i--)
	{	
		double d1;
		GetDataByIdx(i, d1);
		if ((d1 - dLast) > 2)
		{
			keyPointMap[i] = d1;
			dLast = d1;
		}	
	}

	int nLastSet = 0;
	vector<int> arrIn;
	int nLastIdx = 0;
	for (auto iter = keyPointMap.begin(); iter != keyPointMap.end(); iter++)
	{
		arrIn.push_back(iter->second);
		if (arrIn.size() == 2)
		{
			int nCurIdx = iter->first;
			vector<int> outArr;
			MakeLineVal(arrIn[0], arrIn[1], nCurIdx - nLastIdx, false, true, outArr);
			arrIn.clear();
			arrIn.push_back(iter->second);
			for (int i = nLastIdx; i < nCurIdx; i++)
			{
				//assert(nLastSet != outArr[i - nLastIdx]);
				nLastSet = outArr[i - nLastIdx];
				SetDataByIdx(i, nLastSet);
			}
		}
		nLastIdx = iter->first;
	}
}

long CCheFileData::OffsetHPos(vector<DWORD>& arrdatas, int nMidPos, int nPosOffset1, int nPosOffset2, double fZoom)
{
	nMidPos += 10;
	int nIdx = 0;	
	int nDiff = nPosOffset2 - nPosOffset1;
 	double nstep = (nPosOffset2 - nPosOffset1) * 1.0 / (arrdatas.size() - nMidPos);
	double nof = 1;
	int nMax = 0;
	int nLast = 0;
	long lLastVal = -1000;
	double lH = DataToValue(arrdatas[nMidPos]);
	double lM = DataToValue(arrdatas[0]);
	double lHnew = lH * fZoom;
	for (auto iter = arrdatas.begin(); iter != arrdatas.end(); iter++)
	{
		long l = DataToValue(*iter);
		l = ((double)l - lM) * 1.0 / (lH - lM) * lHnew + lM;
		l += nPosOffset1;
		if (l > nMax)
		{
			nMax = l;
		}
		
		if (nIdx++ > nMidPos)
		{
			nof += nstep;
			l += nof;
			if (l > nMax)
			{
				l = nMax - nstep;
				if (nLast != 0)
				{
					if (nLast == l)
					{
						l -= nDiff;
					}
				}
				nLast = l;
			}

			if (lLastVal - l < 1)
			{
				l = lLastVal + (l - lLastVal)/2;
			}
		}
		*iter = ValueToData(l);
		lLastVal = l;
		//TRACE(L"%d\n", l);
	}
	return nMax;
}

bool CCheFileData::FixWave(int nWaveIdx, const char* sFile)
{
	sWaveInfo wi;
	if (!LoadWave(sFile, wi))
	{
		return false;
	}

	sJfItem2 & item1 = m_sCheData.sJfData2.verItems[nWaveIdx];
	int nOffsetH = item1.nTopHFrom;
	long lMax = OffsetHPos(wi.datas, wi.item.nTopDataIdx - wi.item.nBeginDataIdx, nOffsetH, item1.nTopHTo, (double)item1.nTopHVal * 1.0 / wi.item.nTopHVal);

	double fNewWaveWidth = IdxToTime(item1.nEndDataIdx - item1.nBeginDataIdx);
	if(ChangeWaveTimeWidth(nWaveIdx, fNewWaveWidth, &wi))
	{
		int nBak = lMax - item1.nTopHVal;
		item1.nTopHVal = lMax;
		item1.nTopHPos = item1.nTopHPos + nBak;

	//	ChangeWaveTop(nWaveIdx, nBak);
	}


	return true;
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

void CCheFileData::FillStep(int nIdxFrom, int nIdxTo, int nTopFrom, int nTopTo)
{
	int nTopDiff = nTopTo - nTopFrom;
	int nIdxDiff = nIdxTo - nIdxFrom;
	double dNode = (double)nTopDiff / nIdxDiff;
}

bool CCheFileData::ChangeWaveTop(int nIdx, int nTop)
{
	if (nIdx >= GetWaveCnt() || nTop  < 1)
	{
		return false;
	}
	sJfItem & sItem1 = m_sCheData.sJfData.verItems.at(nIdx);
	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nIdx);
	int nleftH = sItem.nTopHVal;// - sItem.nTopHFrom;
	int nrightH = sItem.nTopHVal;// - sItem.nTopHTo;

	if (nleftH < nrightH)
	{
		nrightH = nleftH;
	}
	else
	{
		nleftH = nrightH;
	}

	double fZoom = (double)nTop / sItem.nTopHVal;

	int nleftNewH = nTop - sItem.nTopHFrom;
	int nrightNewH = nTop - sItem.nTopHTo;

// 	int nleftNewH = nTop - sItem.nTopHFrom < sItem.nTopHTo ? sItem.nTopHFrom : sItem.nTopHTo;
// 	int nrightNewH = nTop - sItem.nTopHFrom < sItem.nTopHTo ? sItem.nTopHFrom : sItem.nTopHTo;

	int nIdxFrom = sItem.nBeginDataIdx;
	int nIdxTo = sItem.nTopDataIdx;
	double dMin = 0;
	GetDataByIdx(sItem.nBeginDataIdx, dMin);
	vector<double> arrDataZoom;
	double dLast = -9999;
	double nMaxPos = 0;
	for (int i = nIdxFrom ; i <= nIdxTo; i++)
	{
		double dVal = 0;
		if (GetDataByIdx(i, dVal))
		{
			double fZoomV = (dVal - sItem.nTopHFrom) / nleftH;
			double dNew = nleftNewH * fZoomV + sItem.nTopHFrom;
			if (dNew < dMin)
			{
				dNew = dMin;
			}
			if (dLast != -9999 && dLast - dNew > 1)
			{
				dNew = dLast - (dLast - dNew)/2;
			}
			SetDataByIdx(i, dNew);
			if (i == nIdxFrom)
			{	
				assert(sItem1.fPowerFrom == dNew);
				assert(sItem1.fPowerFrom == sItem.nTopHFrom);
				sItem1.fPowerFrom = sItem.nTopHFrom;
			}
			dLast = dNew;
			if (dNew > nMaxPos)
			{
				nMaxPos = dNew;
			}
		}
	}

	nIdxFrom = sItem.nTopDataIdx + 1;
	nIdxTo = sItem.nEndDataIdx;
	GetDataByIdx(sItem.nEndDataIdx, dMin);
	dLast = -9999;
	for (int i = nIdxFrom ; i <= nIdxTo; i++)
	{
		double dVal = 0;
		if (GetDataByIdx(i, dVal))
		{
			double fZoomV = (dVal - sItem.nTopHTo) / nrightH;
			double dNew = nrightNewH * fZoomV + sItem.nTopHTo;
			if (dNew < dMin)
			{
				dNew = dMin;
			}
			if (dLast != -9999 && dNew - dLast> 1)
			{
				dNew = dLast + (dNew - dLast)/2;
			}
			SetDataByIdx(i, dNew);
			if (i == nIdxTo)
			{
				assert(sItem1.fPowerTo == sItem.nTopHTo);
				sItem1.fPowerTo = sItem.nTopHTo;
			}
			dLast = dNew;
			if (dNew > nMaxPos)
			{
				nMaxPos = dNew;
			}
		}
	}

	SmoothWave(nIdx);

	sItem.nTopHVal = nTop;
	sItem.nTopHPos = nMaxPos;


	FixWaveBottomLine(nIdx);

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
	int nIdxFrom = TimeToIdx(tFrom);
	int nIdxTo = TimeToIdx(tEnd);
	if (nIdxTo > nRangData - 1)
	{
		CString sTips;
		sTips.Format(L"%.3f", IdxToTime(nRangData - 1));

		m_lastErr = L"超越最大值，最大值应小于" + sTips;
		return -2;
	}

	if (tFrom < 0)
	{
		m_lastErr = L"低于最小值，最小值应大于0值";
		return -2;
	}

	int nConflictIdx = -1;
	for (int i = 0; i < nWaveCnt; i++)
	{
		if (i == butIdx)
		{
			continue;
		}
		sJfItem2 sItem;
		GetWaveByIdx(i, sItem);
		if (nIdxFrom >= sItem.nBeginDataIdx && nIdxFrom <= sItem.nEndDataIdx)
		{
			nConflictIdx = i;
			break;
		}
		if (nIdxTo >= sItem.nBeginDataIdx && nIdxTo <= sItem.nEndDataIdx)
		{
			nConflictIdx = i;
			break;
		}
	}

	if (nConflictIdx >= 0)
	{

	}

	return nConflictIdx;
}

void GetNewTimes(double fRangeFrom, double fRangeTo, double f1, double f2, double fMid, double fNew, double & f3, double & f4)
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

	double fOffset = CCheFileData::IdxToTime(1);
	if (f3 < fRangeFrom)
	{
		f4 += (fRangeFrom + fOffset) - f3;
		f3 = fRangeFrom + fOffset;
		assert(f4 < fRangeTo);
	}

	if (f4 > fRangeTo)
	{
		f3 -= (f4 - (fRangeTo - fOffset));
		f4 = fRangeTo - fOffset;
		assert(f3 > fRangeFrom);
	}
}

bool CCheFileData::ChangeWaveTimeWidth(int nIdx, double tWidth, sWaveInfo * pInputWave)
{
	int nWaveCnt = GetWaveCnt();
	if (nIdx >= nWaveCnt)
	{
		return false;
	}
	
	int nRangeData = GetDataCnt();
	bool bOutRange = true;
	float fRangeFrom = 0;
	float fRangeTo = IdxToTime(nRangeData - 2);
	if (nIdx > 0)
	{
		fRangeFrom = IdxToTime(m_sCheData.sJfData2.verItems.at(nIdx - 1).nEndDataIdx);
	}
	if (nIdx + 1 < nWaveCnt)
	{
		fRangeTo = IdxToTime(m_sCheData.sJfData2.verItems.at(nIdx + 1).nBeginDataIdx);
		bOutRange = false;
	}

	if (tWidth > fRangeTo - fRangeFrom)
	{
		if (bOutRange)
		{
			fRangeTo = fRangeFrom + tWidth + 1;
			int nAddRangeData = TimeToIdx(fRangeTo) - nRangeData;
			if (nAddRangeData > 0)
			{
				AppendDatas(nAddRangeData);
			}		
		}
		else
		{
			m_lastErr.Format(L"输入值超出范围，该值应小于%.3f", (int)((fRangeTo - fRangeFrom)*1000) / 1000.0);
			return false;
		}
	}


	sJfItem2 & sItem = m_sCheData.sJfData2.verItems.at(nIdx);
	double dNewFromTime = 0;
	double dNewToTime = 0;
	GetNewTimes(fRangeFrom, fRangeTo, IdxToTime(sItem.nBeginDataIdx), IdxToTime(sItem.nEndDataIdx), IdxToTime(sItem.nTopDataIdx), tWidth, dNewFromTime, dNewToTime);


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

	if(ChangeWaveTimeRange(nIdx, dNewFromTime, dNewToTime, pInputWave))
	{
		sItem.nBeginDataIdx = TimeToIdx(dNewFromTime);
		sItem.nEndDataIdx = TimeToIdx(dNewToTime);

		auto & si = m_sCheData.sJfData.verItems.at(nIdx);
		si.fTimeFrom = dNewFromTime;
		si.fTimeTo = dNewToTime;
// 		si.fPowerFrom = 0;
// 		si.fPowerTo = 0;
		FixWaveBottomLine(nIdx);
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

void CCheFileData::_ZoomTimeData(vector<int> & arrInData, int nNewCnt, vector<int> & arrOutData)
{
	arrOutData.clear();

	int nInputSize = arrInData.size();
	if ( nInputSize < 1 )
	{
		return;
	}

	if (nInputSize == 1)
	{
		for (int i = 0; i  < nNewCnt; i++)
		{
			arrOutData.push_back(arrInData[0]);
		}
	}
	else if (nInputSize == 2)
	{
		MakeLineVal(arrInData[0], arrInData[1], nNewCnt, true, true, arrOutData);
	}
	else
	{
		int nNewSz = nInputSize;
		double delta= nNewCnt*1.0 / nNewSz;

		double dtotal = 0;
		for(int i = 0; i < nNewSz - 1; i++)
		{
			dtotal += delta;
			int nNodeCnt = (int)(dtotal);
			dtotal -= nNodeCnt;
			if (nNewSz - 2 == i)
			{
				nNodeCnt += (int)(dtotal+delta+0.000001);
			}

			vector<int> arr;
			int nVal = arrInData[i];
			int nValNext = arrInData[i + 1];
			MakeLineVal(nVal, nValNext, nNodeCnt, i == 0 ? true : false, true, arr);
			for (int j = 0; j < arr.size(); j++)
			{
				arrOutData.push_back(arr[j]);
			}
		}

		//TRACE(L"%f\n", dtotal);
	}
	
	assert(arrOutData.size() == nNewCnt);
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
	double dFromHVal =0;
	double dToHVal = 0;
	if(pData->GetDataByIdx(nIdxFrom, dItem))
	{
		nLastVal = dItem;
		dFromHVal = dItem;
	}
	if(pData->GetDataByIdx(nIdxTo, dToHVal))
	{
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

// 	arrOutData[0] = dFromHVal;
// 	arrOutData[arrOutData.size() - 1] = dToHVal;	
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

bool CCheFileData::ChangeWaveTimeRange(int nIdx, double tFrom, double tEnd, sWaveInfo * pInputWave)
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
	vector<int> arrLeftData;
	vector<int> arrRightData;
	int nIdxNewFrom = TimeToIdx(tFrom);
	int nIdxNewTo = TimeToIdx(tEnd);
	int nIdxTop = sItem.nTopDataIdx;
	if (!pInputWave)
	{	
		int nIdxFrom = sItem.nBeginDataIdx;
		int nIdxTo = sItem.nEndDataIdx;
		if (nIdxNewFrom >= nIdxTop)
		{
			return false;
		}
		if (nIdxNewTo <= nIdxTop)
		{
			return false;
		}

		::_ZoomTimeData(this, nIdxFrom, nIdxTop, nIdxTop - nIdxNewFrom, arrLeftData);
		::_ZoomTimeData(this, nIdxTop, nIdxTo, nIdxNewTo - nIdxTop, arrRightData);
	}
	else
	{
		vector<int> larr;
		vector<int> rarr;

		int nlc = (pInputWave->item.nTopDataIdx - pInputWave->item.nBeginDataIdx) + 1;
		int nrc = pInputWave->item.nEndDataIdx - pInputWave->item.nTopDataIdx;

		for (int i = 0; i < pInputWave->item.nEndDataIdx - pInputWave->item.nBeginDataIdx; i++)
		{
			if (i < nlc)
			{
				larr.push_back(DataToValue(pInputWave->datas[i]));
			}
			else
			{
				rarr.push_back(DataToValue(pInputWave->datas[i]));
			}	
		}

		_ZoomTimeData(larr, nIdxTop - nIdxNewFrom, arrLeftData);
		_ZoomTimeData(rarr, nIdxNewTo - nIdxTop, arrRightData);
	}

	NormalizeWave(nIdx);
	
	for (int i = 0; i < arrLeftData.size(); i++)
	{
		SetDataByIdx(i + nIdxNewFrom, arrLeftData[i]);
	}

	int nVal = 0;
	for (int i = 0; i < arrRightData.size(); i++)
	{
		nVal = arrRightData[i];
		SetDataByIdx(i + nIdxTop, nVal);
	}

	SetDataByIdx(arrRightData.size() + nIdxTop, nVal);

	SetDataByIdx(nIdxNewFrom, sItem.nTopHFrom);
	SetDataByIdx(nIdxNewTo, sItem.nTopHTo);

	double dtmp = 0;
	GetDataByIdx(nIdxNewFrom, dtmp);

	sItem.nBeginDataIdx = nIdxNewFrom;
	sItem.nEndDataIdx = nIdxNewTo;

	return true;
}

int	CCheFileData::GetRandomVal(int nFrom, int nTo, int nIdx, int nTimeRange)
{
	int nRange = nTo - nFrom + 1;
	double fnode = (double)nRange / nTimeRange;
	double rval = 0;//(int)rand() % (int)(2*val_offset) - val_offset;
	nFrom += (nIdx * fnode  + rval);
	return nFrom;
}

bool CCheFileData::ChangeWaveTimePos(int nIdx, double tLive, bool bcopyMode)
{
	if (nIdx >= GetWaveCnt())
	{
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

	sJfItem2 sItem;
	GetWaveByIdx(nIdx, sItem);
	int nIdxFromOld = sItem.nBeginDataIdx;
	int nIdxToOld = sItem.nEndDataIdx;

	int nRangeData = GetDataCnt();
	int nNewLiveIdx = TimeToIdx(tLive);
	int nNewDataCnt = nIdxToOld - nIdxFromOld + 1;
	if (nNewLiveIdx + nNewDataCnt > nRangeData)
	{
		int nAddRangeData = nNewLiveIdx + nNewDataCnt - nRangeData;
		AppendDatas(nAddRangeData);
	}

	double tFrom = IdxToTime(nIdxFromOld) + fOffset;
	double tEnd = IdxToTime(nIdxToOld) + fOffset;
	int nIdxBut = bcopyMode ? -1 : nIdx;
	int nConflictIdx = TestTimeRange(tFrom, tEnd, nIdxBut);
	if (nConflictIdx >= 0)
	{
		m_lastErr = L"波形重合，数据文件不支持此类型";
		return false;
	}	

	sJfItem2 & item2 = m_sCheData.sJfData2.verItems[nIdx];
	if(nConflictIdx == -2)
	{
		double flTime = IdxToTime(item2.nTopDataIdx - item2.nBeginDataIdx + 1);
		double frTime = IdxToTime(item2.nEndDataIdx - item2.nTopDataIdx - 1);
		CString sTips;
		if (tFrom < 0)
		{
			sTips.Format(L"%.3f", flTime);
		}
		else
		{
			int nRangData = GetDataCnt();
			sTips.Format(L"%.3f", IdxToTime(nRangData - 1) - frTime);
		}
	
		m_lastErr = L"超越最大值，最大值应小于" + sTips;
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

	if (!bcopyMode)
	{
		NormalizeWave(nIdx);

		sJfItem3 & item = m_sCheData.sJgData.verItems[nIdx];
		item.fLiveTime = (float)tLive;

		m_sCheData.sJfData.verItems[nIdx].fTimeFrom = tFrom;
		m_sCheData.sJfData.verItems[nIdx].fTimeTo = tEnd;

		item2.nBeginDataIdx = nIdxFrom;
		item2.nTopDataIdx = TimeToIdx(tLive);
		item2.nEndDataIdx = nIdxTo;
	}

	for (int i = nIdxFrom; i <= nIdxTo; i++)
	{
		SetDataByIdx(i, pDataBak[i - nIdxFrom]);
	}

	delete [] pDataBak;

	if (bcopyMode)
	{
		sJfItem item1New;
		memcpy(&item1New, &m_sCheData.sJfData.verItems[nIdx], sizeof(item1New));
		item1New.fTimeFrom = tFrom;
		item1New.fTimeTo = tEnd;

		m_sCheData.sJfData.nItemCnt++;
		m_sCheData.sJfData.wItemCnt++;
		m_sCheData.sJfData.verItems.push_back(item1New);

		sJfItem2 item2New;
		memcpy(&item2New, &item2, sizeof(item2));
		item2New.nBeginDataIdx = nIdxFrom;
		item2New.nTopDataIdx = TimeToIdx(tLive);
		item2New.nEndDataIdx = nIdxTo;

		m_sCheData.sJfData2.nItemCnt++;
		m_sCheData.sJfData2.wItemCnt++;
		m_sCheData.sJfData2.verItems.push_back(item2New);

		nIdx = m_sCheData.sJfData2.verItems.size() - 1;
	}

	FixWaveEdge(nIdx);
	FixWaveBottomLine(nIdx);
	return true;
}

void CCheFileData::RemoveWave(int nIdx)
{
	NormalizeWave(nIdx);
	m_sCheData.sJfData2.verItems.erase(m_sCheData.sJfData2.verItems.begin() + nIdx);
	m_sCheData.sJfData2.nItemCnt = m_sCheData.sJfData2.verItems.size();

	m_sCheData.sJfData.verItems.erase(m_sCheData.sJfData.verItems.begin() + nIdx);
	m_sCheData.sJfData.nItemCnt = m_sCheData.sJfData.verItems.size();
}

int	CCheFileData::AddWave(double tLiveTime, double tWidth, int nTopValue, double dSqrt, int nBaseOnIdx)
{
	sJfItem2 item;
	if(!GetWaveByIdx(nBaseOnIdx, item))
	{
		return -1;
	}

	if (!ChangeWaveTimePos(nBaseOnIdx, tLiveTime, true))
	{
		assert(0);
		return -1;
	}

	int nNewItemIdx = GetWaveCnt() - 1;

	do 
	{
		bool bOK = true;
		if (abs(TimeToIdx(tWidth) - (item.nEndDataIdx - item.nBeginDataIdx)) >= 1)
		{
			bOK = ChangeWaveTimeWidth(nNewItemIdx, tWidth);
		}

		if (bOK && abs(nTopValue - item.nTopHVal) >= 1)
		{
			bOK = ChangeWaveTop(nNewItemIdx, nTopValue);
		}

		if (bOK && dSqrt != item.nTopSqrt)
		{
			bOK = ChangeWaveSqrt(nNewItemIdx, dSqrt);
		}

		if (!bOK)
		{
			break;
		}
		return nNewItemIdx;
	} while (false);
	
	RemoveWave(nNewItemIdx);
	return -1;
}

void CCheFileData::AppendTimes(float nTimes)
{
	int nDataCnt = nTimes / IdxToTime(1);
	AppendDatas(nDataCnt);
}

void CCheFileData::AppendDatas(int nTimes)
{
	double dVal;
	if(!GetDataByIdx(m_sCheData.nDataCnt - 1, dVal))
	{
		return;
	}

	int nPosFrom = 0;
	int nWvCnt = GetWaveCnt();
	if (nWvCnt > 0)
	{
		sJfItem2 item;
		GetWaveByIdx(nWvCnt - 1, item);
		nPosFrom = item.nEndDataIdx + 1;
	}
	
	int nMinDiff = 0x10000;
	int nMinPos = 0;
	for(int i = nPosFrom; i < m_sCheData.nDataCnt - 1; i++)
	{
		double dVal2;
		if(!GetDataByIdx(i, dVal2))
		{
			break;
		}

		int nDiff = abs(dVal - dVal2);
		if (nDiff < nMinDiff)
		{
			nMinDiff = nDiff;
			nMinPos = i;
		}

		if (nDiff < 2)
		{
			nMinPos = i;
			break;
		}
	}

	m_sCheData.verMainDatas.reserve(m_sCheData.verMainDatas.size() + nTimes + 1);
	int nCnt = 0;
	for(int i = nMinPos; i < m_sCheData.nDataCnt; i++)
	{
		double dv;
		GetDataByIdx(i, dv);
		m_sCheData.verMainDatas.push_back(ValueToData(dv));
		nCnt++;
		if (nCnt == nTimes)
		{
			break;
		}
		if (i == m_sCheData.nDataCnt - 1)
		{
			i = nMinPos;
		}
	}

	m_sCheData.nDataCnt = m_sCheData.verMainDatas.size();
	*m_sCheData.nDataCnt2 = m_sCheData.nDataCnt;
}

bool CCheFileData::SaveWave(int nIdxWave, const char * sFile)
{
	sJfItem2 item;
	
	if(!GetWaveByIdx(nIdxWave, item))
	{
		return false;
	}

	FILE * fp = fopen(sFile, "wb");
	if (!fp)
	{
		return false;
	}

	fwrite(&item, 1, sizeof(item), fp);

	long lLast = 0;
	DWORD * pDatas = new DWORD[item.nEndDataIdx - item.nBeginDataIdx + 1];
	for (int i = item.nBeginDataIdx; i <= item.nEndDataIdx; i++)
	{
		long lv = DataToValue(m_sCheData.verMainDatas.at(i));
		lv -= item.nTopHFrom;
		if (i <= item.nTopDataIdx)
		{
			if (lv < lLast-3)
			{
				lv = lLast-3 + 1;
			}
		}
		else
		{
			if (lv > lLast+3)
			{
				lv = lLast+3 - 1;
			}
		}
		if (lv < 0)
		{
			lv = 0;
		}
		lLast = lv;
		pDatas[i - item.nBeginDataIdx] = ValueToData(lv);
	}
		
	fwrite(pDatas, 4, item.nEndDataIdx - item.nBeginDataIdx + 1, fp);
	delete [] pDatas;

	fclose(fp);

	return true;
}

bool CCheFileData::LoadWave(const char * sFile, sWaveInfo & info)
{
	FILE * fp = fopen(sFile, "rb");
	if (!fp)
	{
		return false;
	}
	sJfItem2 & item = info.item;

	int ns = fread(&item, 1, sizeof(item), fp);
	assert(ns == sizeof(sJfItem2));

	DWORD * pDatas = new DWORD[item.nEndDataIdx - item.nBeginDataIdx + 1];
	ns = fread(pDatas, 4, info.item.nEndDataIdx - item.nBeginDataIdx + 1, fp);	
	assert(ns == item.nEndDataIdx - item.nBeginDataIdx + 1);
	info.datas.resize(item.nEndDataIdx - item.nBeginDataIdx + 1);
	memcpy((LPBYTE)info.datas.data(), pDatas, ns*4);

	delete [] pDatas;

	fclose(fp);
	return true;
}

void CCheFileData::Clear()
{
	srand(time(NULL));
	m_nSaveDataFromIdx = 0;
	m_nNorLineWidthStep = 5;	
	m_nNorLineHeightStep= 5;

	memset(&m_sCheData.sVer[0], 0, 0x22);
	m_sCheData.nDataCnt = 0;
	m_sCheData.nYLimit_High  = 0;
	m_sCheData.nYLimit_Low = 0;
	m_sCheData.verMainDatas.clear();
	memset(&m_sCheData.unKownBytes_0a[0], 0, 0x0a);
	memset(&m_sCheData.unKownBytes_2e[0], 0, 0x2E);
	m_sCheData.nDataCnt2 = (int*)&m_sCheData.unKownBytes_2e[6];
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
