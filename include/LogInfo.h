#pragma once

/** 日志类型
*/
enum{
	LOG_DEBUG=0,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
};

/** 数据类型
*/
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

/** 时间显示类型
*/
enum{
	TT_DATE_TIME=0,		//year-month-day hour:minute:second 
	TT_TIME,			//hour:minute:second 
	TT_TIME_MSEC,		//hour:minute:second.msec 
	TT_DATE_TIME_MSEC	//year-month-day hour:minute:second.msec
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
		STATE_NONE,
		STATE_CNT
	};

	enum{
		DATA_BUF_LEN = 8192,
		INFO_BUF_LEN = 1024
	};

	void log(LPCSTR sInfo, int iLogType, const BYTE*pData, int len, int iDataType, int iTimeType, unsigned __int64 iInfoID)
	{
		GetLocalTime(&m_stm);

		CString sTime;
		TimeFormat(sTime, m_stm, iTimeType);

		sprintf_s(m_sInfo, 8192, "%s %s", sTime, sInfo);

		m_iLogType = iLogType;
		m_iDataType = iDataType;
		m_iDataLen = 0;
		if(pData!=NULL&&len>0)
		{
			m_iDataLen = len;
			if(m_iDataLen>DATA_BUF_LEN) m_iDataLen=DATA_BUF_LEN;
			//m_pData = new BYTE[len];
			memcpy(m_dataBuf, pData, m_iDataLen);
		}

		m_iInfoID = iInfoID;
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

		m_iLineOpenCnt = 1+m_iDataLen/m_iLineHexCntEx;
		if(m_iDataLen%m_iLineHexCntEx) m_iLineOpenCnt++;
	}

	/** 设置数据是否被打开 when mouse left button down, call this function.
	 */
	void SetIsOpen();

	/** 获得某行的字符串
	 * @param sLine: result
	 * @param iLineIdx:which line in this information
	 */
	void GetLineStr(CString&sLine, int iLineIdx);

	/** 获得类型的字符串
	*/
	void GetTypeStr(CString&sType);

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
	char m_sInfo[INFO_BUF_LEN];

	//!记录时间
	SYSTEMTIME m_stm;

	//!日志的类型
	int m_iLogType;

	//!数据的类型
	int m_iDataType;

	//!数据
	//BYTE*m_pData;
	//!数据长度
	UINT m_iDataLen;
	BYTE m_dataBuf[DATA_BUF_LEN];

	//!行数
	int m_iLineCnt;

	//!打开数据的行数
	int m_iLineOpenCnt;

	//!是否是显示数据状态
	bool m_bOpen;

	//! format
	char m_sFormat[32];

	//! data count of a line 
	int m_iLineHexCnt;
	int m_iLineHexCntEx;

	//! 唯一的信息ID
	unsigned __int64 m_iInfoID;

private:
	void GetLineDataStr(CString&sLine, int iLineIdx);

	/** 将系统时间转化为字符串
	 * @param sTime: output string of time
	 * @param stm : input system time
	 * @param iTimeType: the type of time format
	 */
	void TimeFormat(CString& sTime, const SYSTEMTIME& stm, int iTimeType);

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
			UINT iBeg = iLineIdx*m_iLineHexCnt;
			UINT iEnd = iBeg+m_iLineHexCnt;
			if(iEnd>m_iDataLen) iEnd = m_iDataLen;

			sLine.Format(_T("[%08X]"), iLineIdx*m_iLineHexCnt);

			CString sTemp;
			for(UINT i=iBeg; i<iEnd; i++)
			{
				if(i%8==0) sLine+=_T(" ");

				sTemp.Format(m_sFormat, m_dataBuf[i]);
				sLine+=" ";
				//sTemp.Format(" %02X", m_pData[i]);
				sLine+=sTemp;
			}
		}
		break;
	case DATA_HEX16:
		{
			WORD*pData = (WORD*)m_dataBuf;
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
			WORD*pData = (WORD*)m_dataBuf;
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
			DWORD*pData = (DWORD*)m_dataBuf;
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
			DWORD*pData = (DWORD*)m_dataBuf;
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
			unsigned __int64 *pData = (unsigned __int64*)m_dataBuf;
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
			unsigned __int64* pData = (unsigned __int64*)m_dataBuf;
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
			float*pData = (float*)m_dataBuf;
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
			float*pData = (float*)m_dataBuf;
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
			double*pData = (double*)m_dataBuf;
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
			double*pData = (double*)m_dataBuf;
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

inline void CLogInfo::TimeFormat(CString& sTime, const SYSTEMTIME& stm, int iTimeType)
{
	switch(iTimeType)
	{
	case TT_DATE_TIME:
		{
			sTime.Format("%04d-%02d-%02d %02d:%02d:%02d", stm.wYear, stm.wMonth, stm.wDay, 
				stm.wHour, stm.wMinute, stm.wSecond);
		}
		break;
	case TT_TIME:
		{
			sTime.Format("%02d:%02d:%02d", stm.wHour, stm.wMinute, stm.wSecond);
		}
		break;
	case TT_TIME_MSEC:
		{
			sTime.Format("%02d:%02d:%02d.%03d", stm.wHour, stm.wMinute, stm.wSecond, stm.wMilliseconds);
		}
		break;
	case TT_DATE_TIME_MSEC:
		{
			sTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d", stm.wYear, stm.wMonth, stm.wDay, 
				stm.wHour, stm.wMinute, stm.wSecond, stm.wMilliseconds);
		}
		break;
	default:
		{
			sTime.Format("%02d:%02d:%02d", stm.wHour, stm.wMinute, stm.wSecond);
		}
	}
}


/**	用于显示行的信息结构
*/
struct INFO_SHOW
{
	INFO_SHOW()
		: iInfoID(0xFFFFFFFFFFFFFFFF)
		, iLineIdx(0)
		, iLogType(LOG_INFO)
		, iShowState(CLogInfo::STATE_NONE)
		, pLogInfo(NULL)
	{}

	unsigned __int64 iInfoID;	//!<which info
	int iLineIdx;				//!<in this info , which line
	int iLogType;				//!<type of log，is LOG_DEBUG LOG_INFO ...
	int iShowState;				//!< STATE_NONE STATE_CLOSE STATE_OPEN
	CLogInfo*pLogInfo;			//!<pointer of CLogInfo(log information)
	CString szLine;				//!<string of line
};

