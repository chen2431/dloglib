// LogWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogWnd.h"

using namespace std;

// CLogWnd

IMPLEMENT_DYNAMIC(CLogWnd, CWnd)

CLogWnd::CLogWnd()
 : m_iAllLineCnt(0)
 , m_iLineHeight(20)
 , m_bScrollToEnd(true)
 , m_iFontSize(90)
 , m_colorBack(RGB(250,255,240))
 //, m_colorLine(RGB(236,243,252))
 , m_colorLine(RGB(238,250,230))
 , m_colorSel(RGB(204,232,255))
 , m_colorSel1(RGB(220,240,255))
 , m_bChange(true)
 , m_iTopPos(20000)
 , m_iTimeType(0)
 , m_bFileOpen(FALSE)
 , m_rtMem(0,0,0,0)
 , m_iInfoCnt(0)
 , m_iFirstShowLineIdx(0)
 , m_iInfoShowCnt(0)
 , m_iInfoID(0)
 , m_plineCnt(NULL)
 , m_infoShow(NULL)
{
	RegisterWindowClass();

	InitializeCriticalSection(&m_csLock);

	m_scrollHelper = new CScrollHelper;
	m_scrollHelper->AttachWnd(this);

	for(int i=0; i<8; i++)
	{
		m_colorFont[i] = RGB(0,0,96);
	}

	m_colorFont[LOG_DEBUG] = RGB(31,48,62);
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

	m_plineCnt = new UINT[MAX_INFO_CNT];
	m_infoShow = new INFO_SHOW[MAX_SHOW_CNT];
}

CLogWnd::~CLogWnd()
{
	delete m_scrollHelper;
	if(m_bFileOpen)
	{
		m_save.Close();
	}

	EnterCriticalSection(&m_csLock);
	list<CLogInfo*>::iterator it=m_listInfo.begin();
	for(it; it!=m_listInfo.end(); ++it)
	{
		CLogInfo*pLogInfo = (*it);
		delete pLogInfo;
	}
	LeaveCriticalSection(&m_csLock);

	if(m_plineCnt) delete [] m_plineCnt;
	if(m_infoShow) delete [] m_infoShow;

	DeleteCriticalSection(&m_csLock);
	DestroyWindow();
}

BOOL CLogWnd::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, LOGVIEWER_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = LOGVIEWER_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CLogWnd::CreateMultiLevelDirectory(const CString dd)
{
	HANDLE fFile;             // File Handle
	WIN32_FIND_DATA fileinfo; // File Information Structure
	CStringArray m_arr;       // CString Array to hold Directory Structures
	BOOL tt;                  // BOOL used to test if Create Directory was successful
	int x1 = 0;               // Counter
	CString tem = "";         // Temporary CString Object

	//Before we go to a lot of work. Check to see if the Directory exists
	fFile = FindFirstFile(dd,&fileinfo);
	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		// Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}

	m_arr.RemoveAll(); // Not really necessary - Just habit
	//Separate the String into its compounded components
	for(x1 = 0; x1 < dd.GetLength(); x1++ ) // Parse the supplied CString Directory String
	{ 
		if(dd.GetAt(x1) != '\\') // if the Charachter is not a 
			tem += dd.GetAt(x1); // add the character to the Temp String
		else
		{
			m_arr.Add(tem); // if the Character is a \ 
			//Add the Temp String to the CString Array
			tem += "\\"; // Now add the \ to the temp string
		}

		if(x1 == dd.GetLength()-1) // If we reached the end of the String
			m_arr.Add(tem);   //add the remaining string
	}
	// Close the file
	FindClose(fFile);

	//Create each level of the Directory Structure
	// Now lets cycle through the String Array and create each directory in turn
	for(x1 = 1; x1 < m_arr.GetSize(); x1++)
	{
		tem = m_arr.GetAt(x1);
		tt = CreateDirectory(tem,NULL);
		// If the Directory exists it will return a false
		if(tt)
			SetFileAttributes(tem,FILE_ATTRIBUTE_NORMAL);
		// If we were successful we set the attributes to normal
	}

	// Now lets see if the directory was successfully created
	fFile = FindFirstFile(dd,&fileinfo);
	//Check to see if the Directory was created and it actually is a Directory
	m_arr.RemoveAll();
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		// Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}
	else
	{
		// For Some reason the Function Failed Return FALSE
		FindClose(fFile);
		return FALSE;
	}
}


void CLogWnd::InitFile(LPCSTR sPath, LPCSTR sPrefix)
{
	CreateMultiLevelDirectory(sPath);

	CString sLogFile;

	SYSTEMTIME stm;
	GetLocalTime(&stm);

	sLogFile.Format("%s\\%s_%04d%02d%02d_%02d%02d%02d.log", sPath, sPrefix, stm.wYear, stm.wMonth, stm.wDay, stm.wHour, stm.wMinute, stm.wSecond);

	if(m_save.Open(sLogFile, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
	{
		m_bFileOpen = TRUE;
	}
}

BOOL CLogWnd::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL bSucc = CWnd::Create(LOGVIEWER_CLASSNAME, lpszText, dwStyle, rect, pParentWnd, nID);

	ModifyStyle(0, WS_CHILD | WS_VISIBLE |WS_HSCROLL | WS_VSCROLL);

	return bSucc;
}

void CLogWnd::PreSubclassWindow()
{
	for(int i=0; i<8; i++)
	{
		BOOL rlt = m_bmpMark[i].LoadBitmap(IDB_MARK_1+i);
		ASSERT(rlt);
	}

	m_bmpState[CLogInfo::STATE_OPEN].LoadBitmap(IDB_OPEN);
	m_bmpState[CLogInfo::STATE_CLOSE].LoadBitmap(IDB_CLOSE);
	m_bmpState[CLogInfo::STATE_NONE].LoadBitmap(IDB_NONE);


	m_menu.LoadMenu(IDR_MENU1);

	

	SetTimer(1001, 100, NULL);
}


BEGIN_MESSAGE_MAP(CLogWnd, CWnd)
	//ON_WM_MOUSEACTIVATE()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_BOTTOM, &CLogWnd::OnMenuBottom)
	ON_COMMAND(ID_MENU_CLEAR, &CLogWnd::OnMenuClear)
END_MESSAGE_MAP()


// CLogWnd ��Ϣ�������
void CLogWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	OnDraw(&dc);
}

void CLogWnd::OnDraw(CDC*pDC)
{
	CDC memoryDC;
	if ( m_rtMem.Width() > 0 && m_rtMem.Height() > 0 &&
		memoryDC.CreateCompatibleDC(pDC) )
	{
		CBitmap bitmap;
		if ( bitmap.CreateCompatibleBitmap(pDC, m_rtMem.Width(), m_rtMem.Height()) )
		{
			CDC* pMemDC = &memoryDC;
			// Select bitmap into memory DC.
			CBitmap* pOldBitmap = pMemDC->SelectObject(&bitmap);

			// draw in memory
			OnDrawMem(pMemDC, m_rtMem);

			// Do the bitblt.
			pDC->BitBlt(0, 0, m_rtClient.Width(), m_rtClient.Height(),
				pMemDC, m_iOffsetX, m_iOffsetY, SRCCOPY);

			pMemDC->SelectObject(pOldBitmap);
		}
	}
}
#include <afxdrawmanager.h>

void CLogWnd::OnDrawMem(CDC*pDC, CRect& rect)
{
	//����
	//pDC->FillSolidRect(rect, m_colorBack);

	CDrawingManager dm(*pDC);
	//dm.Fill4ColorsGradient(rectClient, clr1, clr2, clr2, clr1, !bIsHorz);
	//dm.FillGradient2(rect, RGB(250, 255, 240), RGB(250,240,255), 270);

	//CRect rcRight = rect;rcRight.left = 36;
	dm.Fill4ColorsGradient(rect, RGB(252,255,246), RGB(250, 255, 240), 
		RGB(250, 255, 240), RGB(252, 255, 246));

	CRect rcLeft = rect;
	rcLeft.right = rcLeft.left+36;
	pDC->FillSolidRect(rcLeft, RGB(245, 250, 232));
	//dm.FillGradient2(rcLeft, RGB(250, 255, 240), RGB(232, 232, 228), 180);
	dm.DrawLine(rcLeft.right, rcLeft.top, rcLeft.right, rcLeft.bottom, m_colorLine);

	//����ÿ�л�ͼ
	for(UINT i=0; i<m_iInfoShowCnt; i++)
	{
		CRect rcLine(0, i*m_iLineHeight, rect.Width(), (i+1)*m_iLineHeight);

		CRect rcMark = rcLine; rcMark.right = rcMark.left+20;
		CRect rcState = rcLine;rcState.left = rcMark.right; rcState.right = rcState.left+20;
		CRect rcTxt = rcLine; rcTxt.left = rcState.right;

		//���ѡ�����Ϣ������
		if(m_infoShow[i].iInfoID==m_infoSel.iInfoID)
		{
			if(m_infoSel.iLineIdx==0)
			{
				if(m_infoShow[i].iLineIdx>0)
				{//��ʾ����
					pDC->FillSolidRect(rcTxt, m_colorSel);
				}
				else
				{
					dm.Fill4ColorsGradient(rcTxt, m_colorSel1, m_colorSel, m_colorSel, 
						m_colorSel1, TRUE, 24);
				}
			}
			else if(m_infoSel.iLineIdx==m_infoShow[i].iLineIdx)
			{//��ʾ����
				//pDC->FillSolidRect(rcTxt, m_colorSel);
				dm.Fill4ColorsGradient(rcTxt, m_colorSel1, m_colorSel, m_colorSel, 
					m_colorSel1, TRUE, 24);
			}
		}


		if(m_infoShow[i].iLineIdx==0)
		{
			//bitmap of mark
			{
				CDC memDC;
				memDC.CreateCompatibleDC(pDC);
				CBitmap*pOld = memDC.SelectObject(&m_bmpMark[m_infoShow[i].iLogType]);
				pDC->TransparentBlt(rcMark.left+2, rcMark.top+2, 16, 16, &memDC, 0, 0, 16,16, 0xFFFFFF);
				memDC.SelectObject(pOld);
			}

			//bitmap of state
			int iShowState = m_infoShow[i].iShowState;
			if(iShowState<CLogInfo::STATE_CNT)
			{
				CDC memDC;
				memDC.CreateCompatibleDC(pDC);
				CBitmap*pOld = memDC.SelectObject(&m_bmpState[iShowState]);
				pDC->TransparentBlt(rcState.left+2, rcState.top+2, 16, 16, &memDC, 0, 0, 16,16, 0xFFFFFF);
				memDC.SelectObject(pOld);
			}

			if(rcLine.top>0)
			{
				if(m_infoShow[i].iInfoID!=m_infoSel.iInfoID)
				{
					CPen pen;
					//pen.CreatePen(PS_DOT, 1, m_colorLine);
					pen.CreatePen(PS_SOLID, 1, m_colorLine);
					CPen*pOldPen = pDC->SelectObject(&pen);
					pDC->MoveTo(rcLine.left+16, rcLine.top);
					pDC->LineTo(rcLine.right-16, rcLine.top);
					pDC->SelectObject(pOldPen);
				}
			}
		}

		int old = pDC->SetBkMode(TRANSPARENT);
		COLORREF colorOld = pDC->SetTextColor(m_colorFont[m_infoShow[i].iLogType]);

		CFont font;
		if(m_infoShow[i].iLineIdx==0)
		{
			font.CreatePointFont(m_iFontSize, "΢���ź�");
		}
		else
		{
			//font.CreatePointFont(m_iFontSize+10, "courier new");
			font.CreatePointFont(m_iFontSize+10, "����");
    	}

		CFont*pOldFont = pDC->SelectObject(&font);

		pDC->DrawText(m_infoShow[i].szLine, rcTxt, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		pDC->SelectObject(pOldFont);
		pDC->SetTextColor(colorOld);
		pDC->SetBkMode(old);
	}
}



void CLogWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_scrollHelper->OnHScroll(nSBCode, nPos, pScrollBar);

	m_bChange = true;

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

	m_bChange = true;

	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CLogWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	BOOL wasScrolled = m_scrollHelper->OnMouseWheel(nFlags, zDelta, pt);

	m_bChange = true;

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

	m_scrollHelper->OnSize(nType, cx, cy);

	m_bChange = true;
}

void CLogWnd::UpdateDisplayHeight()
{
	//��Ҫ��ʾ���ܸ߶�
	int iDisplayHeight = m_iAllLineCnt*m_iLineHeight;

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

void CLogWnd::CalculateShowLineCnt()
{
	//��ǰ��������λ��
	CSize pos = m_scrollHelper->GetScrollPos();

	//X�᷽�����ʼλ��
	m_iOffsetX = pos.cx;

	//��ǰӦ����ʾ���м�¼
	m_iFirstShowLineIdx = pos.cy/m_iLineHeight;

	//Y�᷽�����ʼλ��
	m_iOffsetY = pos.cy%m_iLineHeight;

	//ʵ�ʵ���ʾ����
	GetClientRect(m_rtClient);

	//������ʾ����Ŀ����
	m_iShowLineCnt  = m_rtClient.Height()/m_iLineHeight;
	if(m_rtClient.Height()%m_iLineHeight) m_iShowLineCnt+=2;	//����ʾ�����������ϱ߰������±߰���

	//�����ڴ�������Ĵ�Сmemory rect
	m_rtMem.right = m_rtClient.Width();
	m_rtMem.bottom = m_iShowLineCnt*m_iLineHeight;
}

void CLogWnd::CalculateLineCnt()
{
	m_iInfoCnt = m_listInfo.size();

	m_iAllLineCnt = 0;
	int i=0;
	list<CLogInfo*>::iterator it = m_listInfo.begin();
	for(it; it!=m_listInfo.end(); ++it)
	{
		m_iAllLineCnt+=(*it)->m_iLineCnt;
		m_plineCnt[i] = m_iAllLineCnt;

		i++;
	}
}

void CLogWnd::PickShowInfo(UINT iShowLineCnt, UINT iLineIdx)
{
	m_iInfoShowCnt=0;

	int iBegIdx=0;	//��ʼ���Ǹ� info
	int iBegCnt=0;	//�����info ��ǰ�漸������
	for(UINT i=0; i<m_iInfoCnt; i++)
	{
		if(m_plineCnt[i]>iLineIdx)
		{
			iBegIdx = i;
			iBegCnt = m_plineCnt[i]-iLineIdx;
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
				int iLineInnerIdx = pLogInfo->m_iLineCnt-iBegCnt+j;

				m_infoShow[m_iInfoShowCnt].pLogInfo = pLogInfo;

				m_infoShow[m_iInfoShowCnt].iInfoID = pLogInfo->m_iInfoID;
			//	m_infoShow[m_iInfoShowCnt].iInfoIdx = iBegIdx;
				m_infoShow[m_iInfoShowCnt].iLineIdx = iLineInnerIdx;
				m_infoShow[m_iInfoShowCnt].iLogType = pLogInfo->m_iLogType;
				m_infoShow[m_iInfoShowCnt].iShowState = pLogInfo->GetShowState();
				pLogInfo->GetLineStr(m_infoShow[m_iInfoShowCnt].szLine, iLineInnerIdx);

				m_iInfoShowCnt++;

				if(m_iInfoShowCnt>=(UINT)iShowLineCnt)
				{
					break;
				}
			}
		}

		if(i>iBegIdx)
		{
			CLogInfo*pLogInfo = (*it);
			for(int j=0; j<pLogInfo->m_iLineCnt; j++)
			{
				m_infoShow[m_iInfoShowCnt].pLogInfo = pLogInfo;
				m_infoShow[m_iInfoShowCnt].iInfoID = pLogInfo->m_iInfoID;
			//	m_infoShow[m_iInfoShowCnt].iInfoIdx = i;
				m_infoShow[m_iInfoShowCnt].iLineIdx = j;
				m_infoShow[m_iInfoShowCnt].iLogType = pLogInfo->m_iLogType;
				m_infoShow[m_iInfoShowCnt].iShowState = pLogInfo->GetShowState();
				pLogInfo->GetLineStr(m_infoShow[m_iInfoShowCnt].szLine, j);

				m_iInfoShowCnt++;

				if(m_iInfoShowCnt>=(UINT)iShowLineCnt)
				{
					break;
				}
			}
		}

		if(m_iInfoShowCnt>=(UINT)iShowLineCnt)
		{
			break;
		}

		i++;
	}
}

void CLogWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
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
	
	if(m_iLineMemSel<m_iInfoShowCnt)
	{
		CLogInfo*pLogInfo = m_infoShow[m_iLineMemSel].pLogInfo;

		if(pLogInfo)
		{
			m_infoSel = m_infoShow[m_iLineMemSel];

			if(m_infoShow[m_iLineMemSel].iLineIdx==0)
			{
				int posx = point.x+iOffsetX;

				//�������Ӻ���ʾ���ݵ�λ��
				if(posx>20&&posx<40)
				{
					if(pLogInfo)
					{
						pLogInfo->SetIsOpen();
						m_bScrollToEnd = false;
					}
				}
			}

			m_bChange = true;
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CLogWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu*pPopup = m_menu.GetSubMenu(0);

	if(pPopup)
	{
		CPoint cursorPos;
		cursorPos.x= GetCurrentMessage()->pt.x;
		cursorPos.y= GetCurrentMessage()->pt.y;
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursorPos.x, cursorPos.y, this, NULL);
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CLogWnd::OnMenuBottom()
{
	m_bScrollToEnd = true;
//	m_bChange = true;
}

void CLogWnd::OnMenuClear()
{
	EnterCriticalSection(&m_csLock);

	list<CLogInfo*>::iterator it=m_listInfo.begin();
	for(it; it!=m_listInfo.end(); ++it)
	{
		CLogInfo*pLogInfo = (*it);

		if(pLogInfo)
		{
			delete pLogInfo;
			pLogInfo = NULL;
		}
	}
	m_listInfo.clear();

	m_iInfoShowCnt = 0;

	m_bChange = true;
	LeaveCriticalSection(&m_csLock);
}

inline void CLogWnd::Log(LPCSTR sInfo, int iLogType, const BYTE*pData, int iDataLen, int iDataType)
{
	CLogInfo* pLogInfo = new CLogInfo;
	pLogInfo->log(sInfo, iLogType, pData, iDataLen, iDataType, m_iTimeType, m_iInfoID++);
	pLogInfo->SetLineDataCnt(m_iLineHexCnt[iDataType]);
	pLogInfo->SetFormat(m_sFormat[iDataType]);
	
	EnterCriticalSection(&m_csLock);

	if(m_listInfo.size()>MAX_INFO_CNT)
	{
		CLogInfo* pLogInfoEx  = m_listInfo.front();

		if(pLogInfoEx)
		{
			delete pLogInfoEx;
			pLogInfoEx = NULL;
		}

		m_listInfo.pop_front();
	}

	m_listInfo.push_back(pLogInfo);

	m_bChange = true;

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

	LeaveCriticalSection(&m_csLock);
}


void CLogWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(1001==nIDEvent)
	{
		EnterCriticalSection(&m_csLock);
		if(m_bChange)
		{
			//����������
			CalculateLineCnt();

			//�����������㲢���ù��������ܸ߶�
			UpdateDisplayHeight();

			//������ʾ����
			CalculateShowLineCnt();

			if(m_bScrollToEnd)
			{
				m_scrollHelper->ScrollToEnd();
			}

			PickShowInfo(m_iShowLineCnt, m_iFirstShowLineIdx);

			CDC*pDC = GetDC();
			OnDraw(pDC);
			ReleaseDC(pDC);

			m_bChange = false;
		}
		LeaveCriticalSection(&m_csLock);
	}

	CWnd::OnTimer(nIDEvent);
}


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


