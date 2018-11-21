#pragma once
#include "ScrollHelper.h"
#include "LogInfo.h"
#include <list>
#include <vector>

#ifndef _AFXEXT
#ifdef _DEBUG
#pragma comment(lib, "dlogLibD.lib")
#pragma message("dlogLib libary debug version linked.")
#else
#pragma comment(lib, "dlogLib.lib")
#pragma message("dlogLib libary release version linked.")
#endif
#endif

using namespace std;

/** 记录信息定位的结构
 */
struct INFO_IDX
{
	INFO_IDX():iInfoIdx(-1),iLineIdx(-1), pLogInfo(NULL){}

	void Reset(){iInfoIdx = -1; iLineIdx=-1; pLogInfo=NULL;}

	int iInfoIdx;		//!<which info
	int iLineIdx;		//!<in this info , which line
	CLogInfo*pLogInfo;	//!<pointer of CLogInfo(log information)
};

// CLogWnd

class AFX_EXT_CLASS CLogWnd : public CWnd
{
	DECLARE_DYNAMIC(CLogWnd)

public:
	CLogWnd();
	virtual ~CLogWnd();

	/** 设置每行显示数据参数
	 * @param iLineDataCnt 每行显示数据的数量
	 * @param sFormat 显示格式
	 * @param iDataType 设置的是那个数据类型
	 */
	void SetDataShowParam(int iLineDataCnt, LPCSTR sFormat, int iDataType);

	/** 设置每行显示数据的数量
	 * @param iLineDataCnt 每行显示数据的数量
	 * @param iDataType 设置的是那个数据类型
	 */
	void SetLineDataCnt(int iLineDataCnt, int iDataType);

	/** 设置每行显示格式
	 * @param sFormat 显示格式
	 * @param iDataType
	 */
	void SetFormat(LPCSTR sFormat, int iDataType);

	//日志显示
	void LogInfo(LPCSTR sInfo, int iLogType, BYTE*pData=NULL, int len=0, int iDataType=0);

protected:
	//!滚动控制
	CScrollHelper*m_scrollHelper;

	//!是否有变化
	bool m_bChange;

	//!是否滚动到下部
	bool m_bScrollToEnd;

	//!行数
	int m_iLineCnt;

	//!行高低
	int m_iLineHeight;

	//!背景颜色
	COLORREF m_colorBack;
	//!隔离线颜色
	COLORREF m_colorLine;
	//!选择的颜色
	COLORREF m_colorSel;

	//!信息类型的文字颜色
	COLORREF m_colorFont[8];

	//!信息类型的位图
	CBitmap m_bmpMark[8];

	//!是否有数据、是否显示数据状态位图
	CBitmap m_bmpState[3];

	//!字体大小
	int m_iFontSize;

	//!上次距离底部的位置，永远控制进入底部状态
	int m_iTopPos;

	//! format
	CString m_sFormat[DATA_TYPE_CNT];

	//! data count of a line 
	int m_iLineHexCnt[DATA_TYPE_CNT];

	//////////////////////////////////////////////////////////////////////////
	//!数据
	list<CLogInfo*> m_listInfo;

	//行数索引
	vector<int> m_vLineCnt;

	//!当前显示的数据索引
	vector<INFO_IDX> m_vInfoIdx;

	//!当前鼠标选择的那条信息，那行
	INFO_IDX m_infoSel;	

	//!那行,显示中的那行
	int m_iLineMemSel;

	void OnDraw(CDC*pDC);
	void OnDrawMem(CDC*pDC, CRect& rect, int iShowLineCnt, int idx);

	void pop();
	void UpdateLine();
	void UpdateLineCnt();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
};

inline void CLogWnd::SetDataShowParam(int iLineDataCnt, LPCSTR sFormat, int iDataType)
{
	if(iDataType<DATA_TYPE_CNT)
	{
		if(iDataType==DATA_HEX8)
		{
			if(iLineDataCnt<8) iLineDataCnt=8;

			int cnt = iLineDataCnt/8;
			m_iLineHexCnt[iDataType] = cnt*8;
		}
		else 
		{
			if(iLineDataCnt<4) iLineDataCnt=4;

			int cnt = iLineDataCnt/4;
			m_iLineHexCnt[iDataType] = cnt*4;
		}

		m_sFormat[iDataType] = sFormat;
	}
}

inline void CLogWnd::SetLineDataCnt(int iLineDataCnt, int iDataType)
{
	if(iDataType<DATA_TYPE_CNT)
	{
		if(iDataType==DATA_HEX8)
		{
			if(iLineDataCnt<8) iLineDataCnt=8;

			int cnt = iLineDataCnt/8;
			m_iLineHexCnt[iDataType] = cnt*8;
		}
		else 
		{
			if(iLineDataCnt<4) iLineDataCnt=4;

			int cnt = iLineDataCnt/4;
			m_iLineHexCnt[iDataType] = cnt*4;
		}
	}
}

inline void CLogWnd::SetFormat(LPCSTR sFormat, int iDataType)
{
	if(iDataType<DATA_TYPE_CNT)
	{
		m_sFormat[iDataType] = sFormat;
	}
}



