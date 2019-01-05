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

bool CCheFileData::SaveFile()
{
	return true;
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
				*(DWORD*)(&pJfDatas2[nItemFrom] + 0x12)	= si.dwUnknow5;
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

				*(float*)&sTmp[0x00] = sItem.fUnKonw1;
				*(float*)&sTmp[0x04] = sItem.fUnKonw2;
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

		float ftotal = 0;
		if (m_sCheData.nDataCnt > 0)
		{
			float * sDataBuf = new float[m_sCheData.nDataCnt];
			fread(sDataBuf, 4, m_sCheData.nDataCnt, fp);

			for (int i = 0; i < m_sCheData.nDataCnt; i++)
			{
				ftotal += sDataBuf[i];
				m_sCheData.verMainDatas.push_back(sDataBuf[i]);
			}
			delete [] sDataBuf;
		}
		else
		{
			assert(0);
		}

		fread(m_sCheData.unKownBytes_2e, 1, 0x2E, fp);

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
				si.dwUnknow5= *(DWORD*)(&pJfDatas2[nItemFrom] + 0x12);
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

				sItem.fUnKonw1 = *(float*)&sTmp[0x00];
				sItem.fUnKonw2 = *(float*)&sTmp[0x04];
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


void CCheFileData::Clear()
{
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
