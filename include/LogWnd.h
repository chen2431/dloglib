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

#define LOGVIEWER_CLASSNAME    _T("MFCLogViewerCtrl")  // Window class name

// 
/** class CLogWnd
*/
class AFX_EXT_CLASS CLogWnd : public CWnd
{
	DECLARE_DYNAMIC(CLogWnd)

public:
	CLogWnd();
	virtual ~CLogWnd();

	enum{
		LOG_BUF_SIZE = 8091,
		MAX_INFO_CNT = 1000000,	//1000000
		MAX_SHOW_CNT = 4096		//最大显示的行数
	};

	/** 设置保存日志的文件
	 * @param sPath 日志的目录名
	 * @param sPrefix 文件名的前缀
	 */
	void InitFile(LPCSTR sPath, LPCSTR sPrefix=_T("log"));

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

	/** 设置时间显示类型
	 * @param iTimeType: 0: year-month-day hour:minute:second 1: hour:minute:second 2:hour:minute:second.msec 3 year-month-day hour:minute:second.msec
	 */
	void SetTimeType(int iTimeType){ m_iTimeType = iTimeType;}

	/** 调试日志输出
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogDebug(const char*pszFmt, ...);

	/** 信息日志输出
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogInfo(const char*pszFmt, ...);


	/** 警告日志输出
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogWarning(const char*pszFmt, ...);

	/** 错误日志输出
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogError(const char*pszFmt, ...);

	/** 致命错误日志输出
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogFatal(const char*pszFmt, ...);

	/** 调试数据日志显示
	 * @param pData 数据
	 * @param iDataLen 数据长度
	 * @param iDataType 数据类型
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogDataDebug(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);

	/** 信息数据日志显示
	 * @param pData 数据
	 * @param iDataLen 数据长度
	 * @param iDataType 数据类型
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogDataInfo(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);

	/** 警告数据日志显示
	 * @param pData 数据
	 * @param iDataLen 数据长度
	 * @param iDataType 数据类型
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogDataWarning(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);


	/** 错误数据日志显示
	 * @param pData 数据
	 * @param iDataLen 数据长度
	 * @param iDataType 数据类型
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogDataError(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);

	/** 致命错误数据日志显示
	 * @param pData 数据
	 * @param iDataLen 数据长度
	 * @param iDataType 数据类型
	 * @param pszFmt 日志内容
	 * @param ...
	 */
	void LogDataFatal(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);


	/** 数据日志显示
	 * @param iLogType 日志类型
	 * @param pData 数据
	 * @param iDataLen 数据长度
	 * @param sLog 日志内容
	 * @param iDataType 数据类型
	 */
	void LogData(int iLogType, const BYTE*pData, int iDataLen, LPCSTR sLog, int iDataType=0);


	/** 调试日志显示
	 * @param sLog 日志内容
	 * @param pData 数据
	 * @param iDataLen 数据长度 
	 * @param iDataType 数据类型 
	 */
	void LogDebug(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** 信息日志显示
	 * @param sLog 日志内容
	 * @param pData 数据
	 * @param iDataLen 数据长度 
	 * @param iDataType 数据类型 
	 */
	void LogInfo(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** 警告日志显示
	 * @param sLog 日志内容
	 * @param pData 数据
	 * @param iDataLen 数据长度 
	 * @param iDataType 数据类型 
	 */
	void LogWarning(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** 错误日志显示
	 * @param sLog 日志内容
	 * @param pData 数据
	 * @param iDataLen 数据长度 
	 * @param iDataType 数据类型 
	 */
	void LogError(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** 致命错误日志显示
	 * @param sLog 日志内容
	 * @param pData 数据
	 * @param iDataLen 数据长度 
	 * @param iDataType 数据类型 
	 */
	void LogFatal(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);


	/** 日志显示
	 * @param sLog 日志内容
	 * @param iLogType 日志类型 0:调试 1:信息 2:警告 3:错误 4:致命错误 
	 * @param pData 数据
	 * @param iDataLen 数据长度 
	 * @param iDataType 数据类型 
	 */
	void Log(LPCSTR sLog, int iLogType, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** 重载函数
	*/
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);

private:

	/**登记窗口
	*/
	BOOL CLogWnd::RegisterWindowClass();

	/** 生成目录
	*/
	BOOL CreateMultiLevelDirectory(const CString dd); 

protected:
	//!滚动控制
	CScrollHelper*m_scrollHelper;

	//! save log file
	CStdioFile m_save;

	//! is file open
	BOOL m_bFileOpen;

	//!发送数据的锁
	CRITICAL_SECTION m_csLock;

	//!是否有变化
	bool m_bChange;

	//!是否滚动到下部
	bool m_bScrollToEnd;

	//!显示时间的类型
	int m_iTimeType;

	CMenu m_menu;		//!< menu for edit chart parameter

protected:

	//////////////////////////////////////////////////////////////////////////
	//!数据
	std::list<CLogInfo*> m_listInfo;

	//行数索引，用于保存当前信息（包括当前信息）之前的总行数
	UINT *m_plineCnt;//[MAX_INFO_CNT];

	//!当前显示的数据索引
	INFO_SHOW *m_infoShow;//[MAX_SHOW_CNT];

	//!显示信息的行数
	UINT m_iInfoShowCnt;

	//!当前鼠标选择的那条信息，那行
	INFO_SHOW m_infoSel;

	//!信息数量
	UINT m_iInfoCnt;

	//!总行数
	UINT m_iAllLineCnt;

	//!那行,显示中的那行
	UINT m_iLineMemSel;

	//信息计数
	unsigned __int64 m_iInfoID;

protected:

	//显示的第一行是那一行
	UINT m_iFirstShowLineIdx;

	//!显示的行数
	UINT m_iShowLineCnt;

	//! 实际客户区
	CRect m_rtClient;

	//!在内存中的显示区域，大于等于客户区
	CRect m_rtMem;

	//!在内城区中实际显示的位置X
	int m_iOffsetX;

	//!在内城区中实际显示的位置Y
	int m_iOffsetY;

protected:
	//!行高
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

protected:

	/** draw in dc
	*/
	void OnDraw(CDC*pDC);

	/** draw in memory dc
	*/
	void OnDrawMem(CDC*pDC, CRect& rect);

	/** 计算显示的总行数
	*/
	void CalculateLineCnt();

	/** 根据行数改变显示高度
	 */
	void UpdateDisplayHeight();

	/** 计算显示行数
	*/
	void CalculateShowLineCnt();

	/** 挑选显示的信息
	*/
	void PickShowInfo(UINT iShowLineCnt, UINT iLineIdx);

protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMenuBottom();
	afx_msg void OnMenuClear();
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



