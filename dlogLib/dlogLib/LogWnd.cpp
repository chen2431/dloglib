// LogWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogWnd.h"

using namespace std;

// CLogWnd

IMPLEMENT_DYNAMIC(CLogWnd, CWnd)

CLogWnd::CLogWnd()
 : m_iLineCnt(0)
 , m_iLineHeight(20)
 , m_bScrollToEnd(true)
 , m_iFontSize(90)
 , m_colorBack(RGB(255,255,255))
 , m_colorLine(RGB(232,250,255))
 , m_colorSel(RGB(204,232,255))
 , m_bChange(false)
 , m_iTopPos(20000)
 , m_iTimeType(0)
 , m_bFileOpen(FALSE)
{
	InitializeCriticalSection(&m_csLock);

	m_scrollHelper = new CScrollHelper;
	m_scrollHelper->AttachWnd(this);

	for(int i=0; i<8; i++)
	{
		m_colorFont[i] = RGB(0,0,96);
	}

	m_colorFont[LOG_DEBUG] = RGB(32,32,32);
	m_colorFont[LOG_INFO] = RGB(0,0,96);
	m_colorFont[LOG_WARN] = RGB(139,69,0);
	m_colorFont[LOG_ERROR] = RGB(168,96,96);
	m_colorFont[LOG_ERROR] = RGB(255,0,0);

	m_sFormat[DATA_HEX8]	= _T("%02X");
	m_sFormat[DATA_HEX16]	= _T("%04X");
	m_sFormat[DATA_HEX16B]	= _T("%04X");
	m_sFormat[DATA_HEX32]	= _T("%08X");
	m_sFormat[DATA_HEX32B]	= _T("%08X");
	m_sFormat[DATA_HEX64]	= _T("%016I64X");
	m_sFormat[DATA_HEX64B]	= _T("%016I64X");
	m_sFormat[DATA_FLOAT]	= _T("%0.4f");
	m_sFormat[DATA_FLOATB]	= _T("%0.4f");
	m_sFormat[DATA_DOUBLE]	= _T("%0.8lf");
	m_sFormat[DATA_DOUBLEB]	= _T("%0.8lf");

	m_iLineHexCnt[DATA_HEX8]	= 16;
	m_iLineHexCnt[DATA_HEX16]	= 8;
	m_iLineHexCnt[DATA_HEX16B]	= 8;
	m_iLineHexCnt[DATA_HEX32]	= 8;
	m_iLineHexCnt[DATA_HEX32B]	= 8;
	m_iLineHexCnt[DATA_HEX64]	= 4;
	m_iLineHexCnt[DATA_HEX64B]	= 4;
	m_iLineHexCnt[DATA_FLOAT]	= 8;
	m_iLineHexCnt[DATA_FLOATB]	= 8;
	m_iLineHexCnt[DATA_DOUBLE]	= 4;
	m_iLineHexCnt[DATA_DOUBLEB] = 4;
}

CLogWnd::~CLogWnd()
{
	delete m_scrollHelper;
	if(m_bFileOpen)
	{
		m_save.Close();
	}

	list<CLogInfo*>::iterator it=m_listInfo.begin();
	for(it; it!=m_listInfo.end(); ++it)
	{
		CLogInfo*pLogInfo = (*it);
		delete pLogInfo;
	}

	DeleteCriticalSection(&m_csLock);
}


BEGIN_MESSAGE_MAP(CLogWnd, CWnd)
	ON_WM_MOUSEACTIVATE()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CLogWnd::Create(RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CRect rc;

	CWnd*pWnd = pParentWnd->GetDlgItem(nID);
	if(pWnd)
	{
		pWnd->GetWindowRect(rc);
		pParentWnd->ScreenToClient(rc);

		return CWnd::Create(NULL, "CLogWnd", WS_CHILD | WS_VISIBLE |WS_HSCROLL | WS_VSCROLL, rc, pParentWnd, nID);
	}

	return CWnd::Create(NULL, "CLogWnd", WS_CHILD | WS_VISIBLE |WS_HSCROLL | WS_VSCROLL, rect, pParentWnd, nID);
}

// CLogWnd ��Ϣ�������
int CLogWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	ModifyStyle(0, WS_CHILD | WS_VISIBLE |WS_HSCROLL | WS_VSCROLL);

	for(int i=0; i<8; i++)
	{
		BOOL rlt = m_bmpMark[i].LoadBitmap(IDB_MARK_1+i);
		ASSERT(rlt);
	}

	m_bmpState[CLogInfo::STATE_OPEN].LoadBitmap(IDB_OPEN);
	m_bmpState[CLogInfo::STATE_CLOSE].LoadBitmap(IDB_CLOSE);
	m_bmpState[CLogInfo::STATE_NONE].LoadBitmap(IDB_NONE);

	SetTimer(1001, 100, NULL);

	return 0;
}



int CLogWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}


void CLogWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	OnDraw(&dc);
}

void CLogWnd::OnDraw(CDC*pDC)
{
	//��ǰ��������λ��
	CSize pos = m_scrollHelper->GetScrollPos();

	//X�᷽�����ʼλ��
	int iOffsetX = pos.cx;

	//��ǰӦ����ʾ������¼
	int iLineIdx = pos.cy/m_iLineHeight;

	//Y�᷽�����ʼλ��
	int iOffsetY = pos.cy%m_iLineHeight;

	//ʵ�ʵ���ʾ����
	CRect rtClient;
	GetClientRect(rtClient);

	//������ʾ����Ŀ����
	int iLineCnt  = rtClient.Height()/m_iLineHeight;
	if(rtClient.Height()%m_iLineHeight) iLineCnt+=2;	//����ʾ�����������ϱ߰������±߰���

	//�����ڴ�������Ĵ�Сmemory rect
	CRect rect(0, 0, rtClient.Width(), iLineCnt*m_iLineHeight);


	CDC memoryDC;
	if ( rect.Width() > 0 && rect.Height() > 0 &&
		memoryDC.CreateCompatibleDC(pDC) )
	{
		CBitmap bitmap;
		if ( bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()) )
		{
			CDC* pMemDC = &memoryDC;
			// Select bitmap into memory DC.
			CBitmap* pOldBitmap = pMemDC->SelectObject(&bitmap);

			//
			OnDrawMem(pMemDC, rect, iLineCnt, iLineIdx);

			// Do the bitblt.
			pDC->BitBlt(0, 0, rtClient.Width(), rtClient.Height(),
				pMemDC, iOffsetX, iOffsetY, SRCCOPY);

			pMemDC->SelectObject(pOldBitmap);
		}
	}
}

void CLogWnd::OnDrawMem(CDC*pDC, CRect& rect, int iShowLineCnt, int iLineIdx)
{
	//��ǰ��ʾʲô����
	m_vInfoIdx.clear();
	int iInfoCnt = m_vLineCnt.size();
	int iBegIdx=0;
	int iBegCnt=0;
	for(int i=0; i<iInfoCnt; i++)
	{
		if(m_vLineCnt[i]>iLineIdx)
		{
			iBegIdx = i;
			iBegCnt = m_vLineCnt[i]-iLineIdx;
			break;
		}
	}

	int i=0;
	list<CLogInfo*>::iterator it=m_listInfo.begin();
	for(it; it!=m_listInfo.end(); ++it)
	{
		if(i==iBegIdx)
		{
			CLogInfo*pLogInfo = (*it);
			for(int j=0; j<iBegCnt; j++)
			{
				INFO_IDX ii;
				ii.pLogInfo = pLogInfo;
				ii.iInfoIdx = iBegIdx;
				ii.iLineIdx = pLogInfo->m_iLineCnt-iBegCnt+j;
				m_vInfoIdx.push_back(ii);
			}
		}

		if(i>iBegIdx)
		{
			CLogInfo*pLogInfo = (*it);
			for(int j=0; j<pLogInfo->m_iLineCnt; j++)
			{
				INFO_IDX ii;
				ii.pLogInfo = pLogInfo;
				ii.iInfoIdx = i;
				ii.iLineIdx = j;
				m_vInfoIdx.push_back(ii);
			}
		}

		if(m_vInfoIdx.size()>=(UINT)iShowLineCnt)
		{
			break;
		}

		i++;
	}

	//����
	pDC->FillSolidRect(rect, m_colorBack);

	//����ÿ�л�ͼ
	for(int i=0; i<iShowLineCnt; i++)
	{
		CRect rcLine(0, i*m_iLineHeight, rect.Width(), (i+1)*m_iLineHeight);
		
		CRect rcMark = rcLine; rcMark.right = rcMark.left+20;
		CRect rcState = rcLine;rcState.left = rcMark.right; rcState.right = rcState.left+20;
		CRect rcTxt = rcLine; rcTxt.left = rcState.right;

		if(i<(int)m_vInfoIdx.size())
		{
			CLogInfo*pLogInfo = m_vInfoIdx[i].pLogInfo;

			//���ѡ�����Ϣ������
			if(m_vInfoIdx[i].iInfoIdx==m_infoSel.iInfoIdx)
			{
				if(m_infoSel.iLineIdx==0)
				{//��ʾ����
					pDC->FillSolidRect(rcTxt, m_colorSel);
				}
				else if(m_infoSel.iLineIdx==m_vInfoIdx[i].iLineIdx)
				{//��ʾ����
					pDC->FillSolidRect(rcTxt, m_colorSel);
				}
			}
			

			if(m_vInfoIdx[i].iLineIdx==0)
			{
				//bitmap of mark
				{
					CDC memDC;
					memDC.CreateCompatibleDC(pDC);
					CBitmap*pOld = memDC.SelectObject(&m_bmpMark[pLogInfo->m_iLogType]);
					pDC->TransparentBlt(rcMark.left+2, rcMark.top+2, 16, 16, &memDC, 0, 0, 16,16, 0xFFFFFF);
					memDC.SelectObject(pOld);
				}

				//bitmao of state
				int iShowState = m_vInfoIdx[i].pLogInfo->GetShowState();
				if(iShowState<CLogInfo::STATE_CNT)
				{
					CDC memDC;
					memDC.CreateCompatibleDC(pDC);
					CBitmap*pOld = memDC.SelectObject(&m_bmpState[iShowState]);
					pDC->TransparentBlt(rcState.left+2, rcState.top+2, 16, 16, &memDC, 0, 0, 16,16, 0xFFFFFF);
					memDC.SelectObject(pOld);
				}
				//else
				//{
				//	rcTxt.left = rcMark.right+4; 
				//}

				
				CPen pen;

				pen.CreatePen(PS_SOLID, 1, m_colorLine);

				CPen*pOldPen = pDC->SelectObject(&pen);

				pDC->MoveTo(rcLine.left, rcLine.top);
				pDC->LineTo(rcLine.right, rcLine.top);
				pDC->SelectObject(pOldPen);
			}
			
			int old = pDC->SetBkMode(TRANSPARENT);
			COLORREF colorOld = pDC->SetTextColor(m_colorFont[pLogInfo->m_iLogType]);

			CFont font;

			if(m_vInfoIdx[i].iLineIdx==0)
			{
				font.CreatePointFont(m_iFontSize, "΢���ź�");
			}
			else
			{
				//font.CreatePointFont(m_iFontSize+10, "courier new");
				font.CreatePointFont(m_iFontSize+10, "����");
			}
			
			CFont*pOldFont = pDC->SelectObject(&font);

			CString s;
			m_vInfoIdx[i].pLogInfo->GetLineStr(s, m_vInfoIdx[i].iLineIdx);


			pDC->DrawText(s, rcTxt, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

			pDC->SelectObject(pOldFont);

			pDC->SetTextColor(colorOld);
			pDC->SetBkMode(old);
		}
	}
}

void CLogWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_scrollHelper->OnHScroll(nSBCode, nPos, pScrollBar);

	CDC*pDC = GetDC();
	OnDraw(pDC);
	ReleaseDC(pDC);

	//InvalidateRect(NULL);
	//CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLogWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_scrollHelper->OnVScroll(nSBCode, nPos, pScrollBar);


	{//�˲��ִ��룬�����Ƿ����ײ�״̬
		CRect rc;
		GetClientRect(rc);

		//��ʾ�Ĵ�С
		CSize szd = m_scrollHelper->GetDisplaySize();

		//����ײ��ľ���
		int dis = szd.cy-rc.Height()-nPos;

		if(dis<30)
		{//��ʱ����ײ�״̬
			m_bScrollToEnd = true;
		}
		else
		{//����һ��״̬
			if( (nPos>0) && (dis>m_iTopPos))
			{
				m_bScrollToEnd = false;
			}
		}

		//�ϴξ���ײ���λ��
		m_iTopPos = dis;
	}


	InvalidateRect(NULL);
	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CLogWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	BOOL wasScrolled = m_scrollHelper->OnMouseWheel(nFlags, zDelta, pt);
	
	InvalidateRect(NULL);

	return wasScrolled;
}

void CLogWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CRect rect;
	GetWindowRect(&rect);
	CSize sz;
	sz = m_scrollHelper->GetDisplaySize();

	// Initialization: Set the display size if needed.
	if ( m_scrollHelper->GetDisplaySize() == CSize(0,0) )
	{
		if ( rect.Width() > 0 && rect.Height() > 0 )
		{
			m_scrollHelper->SetDisplaySize(rect.Width(), rect.Height());
		}
	}

	UpdateLineCnt();

	m_scrollHelper->OnSize(nType, cx, cy);
}

void CLogWnd::UpdateLineCnt()
{
	int iDisplayHeight = m_iLineCnt*m_iLineHeight;

	//��ǰ��ʾ�Ĵ�С
	CSize sz = m_scrollHelper->GetDisplaySize();

	//�����ж��Ƿ���ˮƽ������
	CSize szPage = m_scrollHelper->GetPageSize();
	if(szPage.cx>0)
	{
		int cySB = ::GetSystemMetrics(SM_CYHSCROLL);

		iDisplayHeight+=cySB;
	}

	m_scrollHelper->SetDisplaySize(sz.cx, iDisplayHeight);
}

void CLogWnd::UpdateLine()
{
	int iSize = m_listInfo.size();
	m_vLineCnt.resize(iSize);

	m_iLineCnt = 0;
	int i=0;
	list<CLogInfo*>::iterator it = m_listInfo.begin();
	for(it; it!=m_listInfo.end(); ++it)
	{
		m_iLineCnt+=(*it)->m_iLineCnt;

		m_vLineCnt[i] = m_iLineCnt;

		i++;
	}

	UpdateLineCnt();

	if(m_bScrollToEnd)
	{
		m_scrollHelper->ScrollToEnd();
	}

	InvalidateRect(NULL);
}

void CLogWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	//��ǰ��������λ��
	CSize pos = m_scrollHelper->GetScrollPos();

	//X�᷽�����ʼλ��
	int iOffsetX = pos.cx;

	//��ǰӦ����ʾ������¼
	int idx = pos.cy/m_iLineHeight;

	//Y�᷽�����ʼλ��
	int iOffsetY = pos.cy%m_iLineHeight;

	//��memRect�е�λ��
	int posy = point.y+iOffsetY;

	//��ǰ��ѡ��
	m_iLineMemSel = posy/m_iLineHeight;
	
	// 
	int cnt = m_vInfoIdx.size();
	if(m_iLineMemSel<cnt)
	{
		CLogInfo*pLogInfo = m_vInfoIdx[m_iLineMemSel].pLogInfo;

		m_infoSel = m_vInfoIdx[m_iLineMemSel];

		if(m_vInfoIdx[m_iLineMemSel].iLineIdx==0)
		{
			int posx = point.x+iOffsetX;

			//�������Ӻ���ʾ���ݵ�λ��
			if(posx>20&&posx<40)
			{
				pLogInfo->SetIsOpen();
				m_bScrollToEnd = false;
				UpdateLine();
			}
		}

		InvalidateRect(NULL);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CLogWnd::LogDebug(LPCSTR sLog)
{
	Log(sLog, LOG_DEBUG);
}
void CLogWnd::LogInfo(LPCSTR sLog)
{
	Log(sLog, LOG_INFO);
}
void CLogWnd::LogWarning(LPCSTR sLog)
{
	Log(sLog, LOG_WARN);
}
void CLogWnd::LogError(LPCSTR sLog)
{
	Log(sLog, LOG_ERROR);
}
void CLogWnd::LogFatal(LPCSTR sLog)
{
	Log(sLog, LOG_FATAL);
}

#define LOG_BUF_SIZE 8191

void CLogWnd::LogDebug(const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_DEBUG);
}
void CLogWnd::LogInfo(const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_INFO);
}
void CLogWnd::LogWarning(const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_WARN);
}
void CLogWnd::LogError(const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_ERROR);
}
void CLogWnd::LogFatal(const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_FATAL);
}

void CLogWnd::LogDataDebug(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_DEBUG, pData, iDataLen, iDataType);
}

void CLogWnd::LogDataInfo(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_INFO, pData, iDataLen, iDataType);
}

void CLogWnd::LogDataWarning(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_WARN, pData, iDataLen, iDataType);
}

void CLogWnd::LogDataError(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_ERROR, pData, iDataLen, iDataType);
}

void CLogWnd::LogDataFatal(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	char szLogBuf[LOG_BUF_SIZE+1];
	if(_vsnprintf_s(szLogBuf, LOG_BUF_SIZE, pszFmt, argp) == -1)
	{
		szLogBuf[LOG_BUF_SIZE] = '\0';
	}

	Log(szLogBuf, LOG_FATAL, pData, iDataLen, iDataType);
}

/** ��־��ʾ
*/
void CLogWnd::LogData(int iLogType, const BYTE*pData, int iDataLen, LPCSTR sInfo, int iDataType)
{
	Log(sInfo, iLogType, pData, iDataLen, iDataType);
}

void CLogWnd::LogDebug(LPCSTR sInfo, const BYTE*pData, int iDataLen, int iDataType)
{
	Log(sInfo, LOG_DEBUG, pData, iDataLen, iDataType);
}

void CLogWnd::LogInfo(LPCSTR sInfo, const BYTE*pData, int iDataLen, int iDataType)
{
	Log(sInfo, LOG_INFO, pData, iDataLen, iDataType);
}

void CLogWnd::LogWarning(LPCSTR sInfo, const BYTE*pData, int iDataLen, int iDataType)
{
	Log(sInfo, LOG_WARN, pData, iDataLen, iDataType);
}

void CLogWnd::LogError(LPCSTR sInfo, const BYTE*pData, int iDataLen, int iDataType)
{
	Log(sInfo, LOG_ERROR, pData, iDataLen, iDataType);
}

void CLogWnd::LogFatal(LPCSTR sInfo, const BYTE*pData, int iDataLen, int iDataType)
{
	Log(sInfo, LOG_FATAL, pData, iDataLen, iDataType);
}

inline void CLogWnd::Log(LPCSTR sInfo, int iLogType, const BYTE*pData, int iDataLen, int iDataType)
{
	CLogInfo* pLogInfo = new CLogInfo;
	pLogInfo->log(sInfo, iLogType, pData, iDataLen, iDataType, m_iTimeType);
	pLogInfo->SetLineDataCnt(m_iLineHexCnt[iDataType]);
	pLogInfo->SetFormat(m_sFormat[iDataType]);
	
	EnterCriticalSection(&m_csLock);

	if(m_listInfo.size()>1000000)
	{
		pop();
	}

	m_listInfo.push_back(pLogInfo);

	m_bChange = true;

	LeaveCriticalSection(&m_csLock);

	//is open
	if(m_bFileOpen)
	{
		CString sLine;
		pLogInfo->GetTypeStr(sLine);
		m_save.WriteString(sLine);
		m_save.WriteString("\n");

		for(int i=0; i<pLogInfo->m_iLineOpenCnt; i++)
		{
			pLogInfo->GetLineStr(sLine, i);
			m_save.WriteString(sLine);
			m_save.WriteString("\n");
		}
		m_save.WriteString("\n");
	}
}

void CLogWnd::pop()
{
	if(m_listInfo.size()>0)
	{
		CLogInfo* pLogInfo  = m_listInfo.front();

		if(pLogInfo) delete pLogInfo;

		m_listInfo.pop_front();

		UpdateLine();
	}
}


void CLogWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(1001==nIDEvent)
	{
		EnterCriticalSection(&m_csLock);
		if(m_bChange)
		{
			UpdateLine();
			m_bChange = false;
		}
		LeaveCriticalSection(&m_csLock);
	}

	CWnd::OnTimer(nIDEvent);
}

BOOL CLogWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ����ר�ô����/����û���
	WNDCLASS wndcls;
	memset(&wndcls, 0, sizeof(WNDCLASS));

	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
	wndcls.lpfnWndProc = ::DefWindowProc; 
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon =  NULL; 
	wndcls.hCursor =NULL;
	wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = _T("LineWnd");
	if(!AfxRegisterClass(&wndcls))
	{
		TRACE("Class Registration Failed!\n");
		return FALSE;
	}
	cs.lpszClass = wndcls.lpszClassName;


	return CWnd::PreCreateWindow(cs);
}


void CLogWnd::PreSubclassWindow()
{
	// TODO: �ڴ����ר�ô����/����û���

	CWnd::PreSubclassWindow();
}

void CLogWnd::InitFile(LPCSTR sFileName)
{
	CString sLogFile;

	SYSTEMTIME stm;
	GetLocalTime(&stm);

	sLogFile.Format("%s_%04d%02d%02d_%02d%02d%02d.log", sFileName, stm.wYear, stm.wMonth, stm.wDay, stm.wHour, stm.wMinute, stm.wSecond);

	if(m_save.Open(sLogFile, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
	{
		m_bFileOpen = TRUE;
	}
}