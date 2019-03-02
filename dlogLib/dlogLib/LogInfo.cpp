#include "StdAfx.h"
#include "LogInfo.h"


CLogInfo::CLogInfo(void)
	: m_iLogType(0)
	, m_iDataType(0)
	, m_iDataLen(0)
	, m_iLineCnt(1)
	, m_bOpen(false)
{

}

CLogInfo::~CLogInfo(void)
{
}

void CLogInfo::SetIsOpen()
{
	if(m_iDataLen>0)
	{
		m_bOpen = !m_bOpen;

		if(m_bOpen)
		{
			m_iLineCnt = 1+m_iDataLen/m_iLineHexCntEx;
			if(m_iDataLen%m_iLineHexCntEx) m_iLineCnt++;
		}
		else
		{
			m_iLineCnt = 1;
		}
	}
}

void CLogInfo::GetLineStr(CString&sLine, int iLineIdx)
{
	if(iLineIdx==0)
	{
		sLine.Format("%s", m_sInfo);
	}
	else
	{
		GetLineDataStr(sLine, iLineIdx-1);
	}
}

#include <string>
void CLogInfo::GetTypeStr(CString&sType)
{
	static std::string s_stype[] = {"`[Debug]", "`[Info]", "`[Warning]", "`[Error]", "`[Fatal]"};
	sType.Format(_T("%s"), s_stype[m_iLogType].c_str());
}