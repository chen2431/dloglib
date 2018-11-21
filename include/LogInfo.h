#pragma once

enum{
	LOG_DEBUG=0,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
};

enum{
	DATA_HEX8=0,	//hex 8bit	
	DATA_HEX16,
	DATA_HEX16B,
	DATA_HEX32,
	DATA_HEX32B,
	DATA_HEX64,
	DATA_HEX64B,
	DATA_FLOAT,
	DATA_FLOATB,
	DATA_DOUBLE,
	DATA_DOUBLEB,
	DATA_TYPE_CNT
};

class CLogInfo
{
public:
	CLogInfo(void);
	~CLogInfo(void);

	enum
	{
		STATE_OPEN=0,
		STATE_CLOSE,
		STATE_NONE
	};

	void log(LPCSTR sInfo, int iLogType, BYTE*pData=NULL, int len=0, int iDataType=0)
	{
		GetLocalTime(&m_stm);

		sprintf_s(m_sInfo, 8192, "%04d-%02d-%02d %02d:%02d:%02d %s",
			m_stm.wYear, m_stm.wMonth, m_stm.wDay, 
			m_stm.wHour, m_stm.wMinute, m_stm.wSecond,
			sInfo);

		m_iLogType = iLogType;
		m_iDataType = iDataType;

		if(pData!=NULL&&len>0)
		{
			m_iDataLen = len;
			m_pData = new BYTE[len];
			memcpy(m_pData, pData, len);
		}
	}

	/**显示格式设置
	*/
	void SetFormat(LPCSTR sFormat)
	{
		sprintf_s(m_sFormat, 32, "%s", sFormat);
	}

	void SetLineDataCnt(int iLineDataCnt)
	{
		m_iLineHexCnt = iLineDataCnt;

		switch(m_iDataType)
		{
		case DATA_HEX8:
			m_iLineHexCntEx = m_iLineHexCnt;
			break;
		case DATA_HEX16:
			m_iLineHexCntEx = m_iLineHexCnt*2;
			break;
		case DATA_HEX16B:
			m_iLineHexCntEx = m_iLineHexCnt*2;
			break;
		case DATA_HEX32:
			m_iLineHexCntEx = m_iLineHexCnt*4;
			break;
		case DATA_HEX32B:
			m_iLineHexCntEx = m_iLineHexCnt*4;
			break;
		case DATA_HEX64:
			m_iLineHexCntEx = m_iLineHexCnt*8;
			break;
		case DATA_HEX64B:
			m_iLineHexCntEx = m_iLineHexCnt*8;
			break;
		case DATA_FLOAT:
			m_iLineHexCntEx = m_iLineHexCnt*4;
			break;
		case DATA_FLOATB:
			m_iLineHexCntEx = m_iLineHexCnt*4;
			break;
		case DATA_DOUBLE:
			m_iLineHexCntEx = m_iLineHexCnt*8;
			break;
		case DATA_DOUBLEB:
			m_iLineHexCntEx = m_iLineHexCnt*8;
			break;
		}
	}

	/** 设置数据是否被打开 when mouse left button down, call this function.
	 */
	void SetIsOpen();

	/** 获得某行的字符串
	 * @param sLine: result
	 * @param iLineIdx:which line in this information
	 */
	void GetLineStr(CString&sLine, int iLineIdx);

	/** 获得显示状态，如果是数据获得数据是否打开
	 * @return 返回显示状态
	 */
	int GetShowState()
	{
		if(m_iDataLen>0)
		{
			if(m_bOpen)
			{
				return STATE_OPEN;
			}
			return STATE_CLOSE;
		}
		return STATE_NONE;
	}

//private:
	char m_sInfo[8192];

	//!记录时间
	SYSTEMTIME m_stm;

	//!日志的类型
	int m_iLogType;

	//!数据的类型
	int m_iDataType;

	//!数据
	BYTE*m_pData;
	//!数据长度
	int m_iDataLen;

	//!行数
	int m_iLineCnt;

	//!是否是显示数据状态
	bool m_bOpen;

	//!每行显示数据的长度
	//int m_iLineDataCnt;

	//! format
	char m_sFormat[32];

	//! data count of a line 
	int m_iLineHexCnt;
	int m_iLineHexCntEx;

private:
	void GetLineDataStr(CString&sLine, int iLineIdx);

};

inline DWORD ChangeEndian(DWORD dwVal)
{
	DWORD dwRlt;
	BYTE*p = (BYTE*)&dwVal;
	dwRlt = (p[0]<<24)+(p[1]<<16)+(p[2]<<8)+p[3];
	return dwRlt;
}

inline unsigned __int64 ChangeEndian(unsigned __int64 llVal)
{
	unsigned __int64 llRlt;
	BYTE* ps = (BYTE*)&llVal;
	BYTE* pd = (BYTE*)&llRlt;

	for(int i=0; i<8; i++)
	{
		pd[i] = ps[7-i];
	}

	return llRlt;
}

inline float ChangeEndian(float fVal)
{
	float fRlt;
	BYTE* ps = (BYTE*)&fVal;
	BYTE* pd = (BYTE*)&fRlt;

	for(int i=0; i<4; i++)
	{
		pd[i] = ps[3-i];
	}

	return fRlt;
}

inline double ChangeEndian(double lfVal)
{
	double lfRlt;
	BYTE* ps = (BYTE*)&lfVal;
	BYTE* pd = (BYTE*)&lfRlt;

	for(int i=0; i<8; i++)
	{
		pd[i] = ps[7-i];
	}

	return lfRlt;
}

inline void CLogInfo::GetLineDataStr(CString&sLine, int iLineIdx)
{
	switch(m_iDataType)
	{
	case DATA_HEX8:
		{
			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>m_iDataLen) iEnd = m_iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt);

			CString sTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				if(i%8==0) sLine+=_T(" ");

				sTemp.Format(m_sFormat, m_pData[i]);
				sLine+=" ";
				//sTemp.Format(" %02X", m_pData[i]);
				sLine+=sTemp;
			}
		}
		break;
	case DATA_HEX16:
		{
			WORD*pData = (WORD*)m_pData;
			int iDataLen = m_iDataLen/2;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*2);

			CString sTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				if(i%8==0) sLine+=_T(" ");

				sTemp.Format(m_sFormat, pData[i]);
				sLine+=" ";
				//sTemp.Format(" %02X", m_pData[i]);
				sLine+=sTemp;
			}
		}
		break;
	case DATA_HEX16B:
		{
			WORD*pData = (WORD*)m_pData;
			int iDataLen = m_iDataLen/2;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*2);

			CString sTemp;
			WORD wTemp;
			BYTE h, l;
			for(int i=iBeg; i<iEnd; i++)
			{
				if(i%8==0) sLine+=" ";

				h = HIBYTE(pData[i]);
				l = LOBYTE(pData[i]);
				wTemp =  (l<<8)|h;

				sTemp.Format(m_sFormat, wTemp);
				sLine+=" ";
				sLine+=sTemp;
			}
		}
		break;
	case DATA_HEX32:
		{
			DWORD*pData = (DWORD*)m_pData;
			int iDataLen = m_iDataLen/4;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*4);

			CString sTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				if(i%8==0) sLine+=_T(" ");
				sTemp.Format(m_sFormat, pData[i]);
				sLine+=" ";
				sLine+=sTemp;
			}
		}
		break;
	case DATA_HEX32B:
		{
			DWORD*pData = (DWORD*)m_pData;
			int iDataLen = m_iDataLen/4;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*4);

			CString sTemp;
			DWORD dwTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				if(i%8==0) sLine+=" ";

				dwTemp = ChangeEndian(pData[i]);

				sTemp.Format(m_sFormat, dwTemp);
				sLine+=_T(" ");
				sLine+=sTemp;
			}
		}
		break;
	case DATA_HEX64:
		{
			unsigned __int64 *pData = (unsigned __int64*)m_pData;
			int iDataLen = m_iDataLen/8;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*8);

			CString sTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				//if(i%8==0) sLine+=" ";
				sTemp.Format(m_sFormat, pData[i]);
				sLine+=" ";
				sLine+=sTemp;
			}
		}
		break;
	case DATA_HEX64B:
		{
			unsigned __int64* pData = (unsigned __int64*)m_pData;
			int iDataLen = m_iDataLen/8;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*8);

			CString sTemp;
			unsigned __int64 llTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				llTemp = ChangeEndian(pData[i]);

				sTemp.Format(m_sFormat, llTemp);
				sLine+=_T(" ");
				sLine+=sTemp;
			}
		}
		break;
	case DATA_FLOAT:
		{
			float*pData = (float*)m_pData;
			int iDataLen = m_iDataLen/4;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*4);

			CString sTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				if(i%8==0) sLine+=_T(" ");
				sTemp.Format(m_sFormat, pData[i]);
				sLine+=_T(" ");
				sLine+=sTemp;
			}
		}
		break;
	case DATA_FLOATB:
		{
			float*pData = (float*)m_pData;
			int iDataLen = m_iDataLen/4;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*4);

			CString sTemp;
			float fTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				if(i%8==0) sLine+=_T(" ");

				fTemp = ChangeEndian(pData[i]);

				sTemp.Format(m_sFormat, fTemp);
				sLine+=_T(" ");
				sLine+=sTemp;
			}
		}
		break;
	case DATA_DOUBLE:
		{
			double*pData = (double*)m_pData;
			int iDataLen = m_iDataLen/8;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*8);

			CString sTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				//if(i%8==0) sLine+=" ";
				sTemp.Format(m_sFormat, pData[i]);
				sLine+=_T(" ");
				sLine+=sTemp;
			}
		}
		break;
	case DATA_DOUBLEB:
		{
			double*pData = (double*)m_pData;
			int iDataLen = m_iDataLen/8;

			int iBeg = iLineIdx*m_iLineHexCnt;
			int iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>iDataLen) iEnd = iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt*8);

			CString sTemp;
			double fTemp;
			for(int i=iBeg; i<iEnd; i++)
			{
				//if(i%8==0) sLine+=" ";

				fTemp = ChangeEndian(pData[i]);

				sTemp.Format(m_sFormat, fTemp);
				sLine+=" ";
				sLine+=sTemp;
			}
		}
		break;
	default:
		{
			;
		}
	}
}

