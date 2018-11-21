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

/** ��¼��Ϣ��λ�Ľṹ
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

	/** ����ÿ����ʾ���ݲ���
	 * @param iLineDataCnt ÿ����ʾ���ݵ�����
	 * @param sFormat ��ʾ��ʽ
	 * @param iDataType ���õ����Ǹ���������
	 */
	void SetDataShowParam(int iLineDataCnt, LPCSTR sFormat, int iDataType);

	/** ����ÿ����ʾ���ݵ�����
	 * @param iLineDataCnt ÿ����ʾ���ݵ�����
	 * @param iDataType ���õ����Ǹ���������
	 */
	void SetLineDataCnt(int iLineDataCnt, int iDataType);

	/** ����ÿ����ʾ��ʽ
	 * @param sFormat ��ʾ��ʽ
	 * @param iDataType
	 */
	void SetFormat(LPCSTR sFormat, int iDataType);

	//��־��ʾ
	void LogInfo(LPCSTR sInfo, int iLogType, BYTE*pData=NULL, int len=0, int iDataType=0);

protected:
	//!��������
	CScrollHelper*m_scrollHelper;

	//!�Ƿ��б仯
	bool m_bChange;

	//!�Ƿ�������²�
	bool m_bScrollToEnd;

	//!����
	int m_iLineCnt;

	//!�иߵ�
	int m_iLineHeight;

	//!������ɫ
	COLORREF m_colorBack;
	//!��������ɫ
	COLORREF m_colorLine;
	//!ѡ�����ɫ
	COLORREF m_colorSel;

	//!��Ϣ���͵�������ɫ
	COLORREF m_colorFont[8];

	//!��Ϣ���͵�λͼ
	CBitmap m_bmpMark[8];

	//!�Ƿ������ݡ��Ƿ���ʾ����״̬λͼ
	CBitmap m_bmpState[3];

	//!�����С
	int m_iFontSize;

	//!�ϴξ���ײ���λ�ã���Զ���ƽ���ײ�״̬
	int m_iTopPos;

	//! format
	CString m_sFormat[DATA_TYPE_CNT];

	//! data count of a line 
	int m_iLineHexCnt[DATA_TYPE_CNT];

	//////////////////////////////////////////////////////////////////////////
	//!����
	list<CLogInfo*> m_listInfo;

	//��������
	vector<int> m_vLineCnt;

	//!��ǰ��ʾ����������
	vector<INFO_IDX> m_vInfoIdx;

	//!��ǰ���ѡ���������Ϣ������
	INFO_IDX m_infoSel;	

	//!����,��ʾ�е�����
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



