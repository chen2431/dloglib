// LogWnd.cpp : 实现文件
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
 , m_colorBack(RGB(250,250,255))
 //, m_colorLine(RGB(236,243,252))
 , m_colorLine(RGB(238,250,230))
 , m_colorSel(RGB(204,232,255))
 //, m_colorSel1(RGB(220,240,255))
 , m_colorSel1(RGB(227,245,255))
 , m_colorSel2(RGB(215,240,255))
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

	//m_colorFont[LOG_DEBUG] = RGB(31,48,62);
	m_colorFont[LOG_DEBUG] = RGB(0,0,96);
	m_colorFont[LOG_INFO] = RGB(0,16,4);
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


// CLogWnd 消息处理程序
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
	//背景
	//pDC->FillSolidRect(rect, m_colorBack);

	CDrawingManager dm(*pDC);
	//dm.Fill4ColorsGradient(rectClient, clr1, clr2, clr2, clr1, !bIsHorz);
	//dm.FillGradient2(rect, RGB(250, 255, 240), RGB(250,240,255), 270);

	//CRect rcRight = rect;rcRight.left = 36;
	dm.Fill4ColorsGradient(rect, RGB(252,255,246), RGB(250, 255, 240), 
		RGB(250, 255, 240), RGB(252, 255, 246));

	CRect rcLeft = rect;
	rcLeft.right = rcLeft.left+40;
	pDC->FillSolidRect(rcLeft, RGB(245, 250, 232));
	//dm.FillGradient2(rcLeft, RGB(250, 255, 240), RGB(232, 232, 228), 180);
	dm.DrawLine(rcLeft.right, rcLeft.top, rcLeft.right, rcLeft.bottom, m_colorLine);

	//按照每行画图
	for(UINT i=0; i<m_iInfoShowCnt; i++)
	{
		CRect rcLine(0, i*m_iLineHeight, rect.Width(), (i+1)*m_iLineHeight);

		CRect rcMark = rcLine; rcMark.right = rcMark.left+20;
		CRect rcState = rcLine;rcState.left = rcMark.right; rcState.right = rcState.left+20;
		CRect rcTxt = rcLine; rcTxt.left = rcState.right+8;
		CRect rcSel = rcLine; rcSel.left = rcState.right+2; rcSel.right-=2;

		//鼠标选择的信息或者行
		if(m_infoShow[i].iInfoID==m_infoSel.iInfoID)
		{
			if(m_infoSel.iLineIdx==0)
			{
				if(m_infoShow[i].iLineIdx>0)
				{//显示多行
					pDC->FillSolidRect(rcSel, m_colorSel);
					dm.DrawLine(rcSel.left-1, rcSel.top-1, rcSel.left-1, rcSel.bottom, m_colorSel);
					dm.DrawLine(rcSel.right, rcSel.top-1, rcSel.right, rcSel.bottom, m_colorSel);
				}
				else
				{
					dm.Fill4ColorsGradient(rcSel, m_colorSel1, m_colorSel2, m_colorSel, 
						m_colorSel1, TRUE, 30);
					dm.DrawLine(rcSel.left-1, rcSel.top+1, rcSel.left-1, rcSel.bottom-2, m_colorSel);
					dm.DrawLine(rcSel.right, rcSel.top+1, rcSel.right, rcSel.bottom-2, m_colorSel);
				}

			}
			else if(m_infoSel.iLineIdx==m_infoShow[i].iLineIdx)
			{//显示单行
				//pDC->FillSolidRect(rcTxt, m_colorSel);
				dm.Fill4ColorsGradient(rcSel, m_colorSel1, m_colorSel2, m_colorSel, 
					m_colorSel1, TRUE, 30);

				dm.DrawLine(rcSel.left-1, rcSel.top+1, rcSel.left-1, rcSel.bottom-2, m_colorSel);
				dm.DrawLine(rcSel.right, rcSel.top+1, rcSel.right, rcSel.bottom-2, m_colorSel);
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
			font.CreatePointFont(m_iFontSize, "微软雅黑");
		}
		else
		{
			//font.CreatePointFont(m_iFontSize+10, "courier new");
			font.CreatePointFont(m_iFontSize+10, "宋体");
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_scrollHelper->OnHScroll(nSBCode, nPos, pScrollBar);

	m_bChange = true;

	//CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLogWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_scrollHelper->OnVScroll(nSBCode, nPos, pScrollBar);

	{//此部分代码，决定是否进入底部状态
		CRect rc;
		GetClientRect(rc);

		//显示的大小
		CSize szd = m_scrollHelper->GetDisplaySize();

		//距离底部的距离
		int dis = szd.cy-rc.Height()-nPos;

		if(dis<30)
		{//这时进入底部状态
			m_bScrollToEnd = true;
		}
		else
		{//进入一般状态
			if( (nPos>0) && (dis>m_iTopPos))
			{
				m_bScrollToEnd = false;
			}
		}

		//上次距离底部的位置
		m_iTopPos = dis;
	}

	m_bChange = true;

	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CLogWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
	//需要显示的总高度
	int iDisplayHeight = m_iAllLineCnt*m_iLineHeight;

	//当前显示的大小
	CSize sz = m_scrollHelper->GetDisplaySize();

	//这里判断是否有水平滚动条
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
	//当前滚动条的位置
	CSize pos = m_scrollHelper->GetScrollPos();

	//X轴方向的起始位置
	m_iOffsetX = pos.cx;

	//当前应该显示那行记录
	m_iFirstShowLineIdx = pos.cy/m_iLineHeight;

	//Y轴方向的起始位置
	m_iOffsetY = pos.cy%m_iLineHeight;

	//实际的显示区域
	GetClientRect(m_rtClient);

	//可以显示的条目数量
	m_iShowLineCnt  = m_rtClient.Height()/m_iLineHeight;
	if(m_rtClient.Height()%m_iLineHeight) m_iShowLineCnt+=2;	//多显示两条，由于上边半条，下边半条

	//申请内存中区域的大小memory rect
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

	int iBegIdx=0;	//开始是那个 info
	int iBegCnt=0;	//在这个info 中前面几个不是
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
	//当前滚动条的位置
	CSize pos = m_scrollHelper->GetScrollPos();

	//X轴方向的起始位置
	int iOffsetX = pos.cx;

	//当前应该显示那条记录
	int idx = pos.cy/m_iLineHeight;

	//Y轴方向的起始位置
	int iOffsetY = pos.cy%m_iLineHeight;

	//在memRect中的位置
	int posy = point.y+iOffsetY;

	//当前的选择
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

				//单击到加号显示数据的位置
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(1001==nIDEvent)
	{
		EnterCriticalSection(&m_csLock);
		if(m_bChange)
		{
			//计算总行数
			CalculateLineCnt();

			//根据行数计算并设置滚动窗口总高度
			UpdateDisplayHeight();

			//计算显示行数
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

/** 日志显示
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


