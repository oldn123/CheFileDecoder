#include "StdAfx.h"
#include "CheFileData.h"
#include <assert.h>

CCheFileData::CCheFileData(void)
{
}


CCheFileData::~CCheFileData(void)
{
}

bool CCheFileData::SaveFile()
{
	return true;
}

bool CCheFileData::SaveFile(LPCTSTR)
{
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
			m_sCheData.nYLimit = *(int*)&sBuf[0x26];
			m_sCheData.nUnKnowVal1 = *(int*)&sBuf[0x2A];
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

		fread(m_sCheData.bufUnKnow1, 1, 0x2E, fp);

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
				si.wUnKnow = *(float*)(&pJfDatas[i * 0x12] + 0x10);

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

				m_sCheData.sJfData2.verItems.push_back(si);
			}

			delete [] pJfDatas2;
		}



	} while (false);



	fclose(fp);


	m_strInputfile = sInput;

	return true;
}