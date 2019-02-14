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
		MAX_SHOW_CNT = 4096		//�����ʾ������
	};

	/** ���ñ�����־���ļ�
	 * @param sPath ��־��Ŀ¼��
	 * @param sPrefix �ļ�����ǰ׺
	 */
	void InitFile(LPCSTR sPath, LPCSTR sPrefix=_T("log"));

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

	/** ����ʱ����ʾ����
	 * @param iTimeType: 0: year-month-day hour:minute:second 1: hour:minute:second 2:hour:minute:second.msec 3 year-month-day hour:minute:second.msec
	 */
	void SetTimeType(int iTimeType){ m_iTimeType = iTimeType;}

	/** ������־���
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogDebug(const char*pszFmt, ...);

	/** ��Ϣ��־���
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogInfo(const char*pszFmt, ...);


	/** ������־���
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogWarning(const char*pszFmt, ...);

	/** ������־���
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogError(const char*pszFmt, ...);

	/** ����������־���
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogFatal(const char*pszFmt, ...);

	/** ����������־��ʾ
	 * @param pData ����
	 * @param iDataLen ���ݳ���
	 * @param iDataType ��������
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogDataDebug(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);

	/** ��Ϣ������־��ʾ
	 * @param pData ����
	 * @param iDataLen ���ݳ���
	 * @param iDataType ��������
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogDataInfo(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);

	/** ����������־��ʾ
	 * @param pData ����
	 * @param iDataLen ���ݳ���
	 * @param iDataType ��������
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogDataWarning(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);


	/** ����������־��ʾ
	 * @param pData ����
	 * @param iDataLen ���ݳ���
	 * @param iDataType ��������
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogDataError(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);

	/** ��������������־��ʾ
	 * @param pData ����
	 * @param iDataLen ���ݳ���
	 * @param iDataType ��������
	 * @param pszFmt ��־����
	 * @param ...
	 */
	void LogDataFatal(const BYTE*pData, int iDataLen, int iDataType, const char*pszFmt, ...);


	/** ������־��ʾ
	 * @param iLogType ��־����
	 * @param pData ����
	 * @param iDataLen ���ݳ���
	 * @param sLog ��־����
	 * @param iDataType ��������
	 */
	void LogData(int iLogType, const BYTE*pData, int iDataLen, LPCSTR sLog, int iDataType=0);


	/** ������־��ʾ
	 * @param sLog ��־����
	 * @param pData ����
	 * @param iDataLen ���ݳ��� 
	 * @param iDataType �������� 
	 */
	void LogDebug(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** ��Ϣ��־��ʾ
	 * @param sLog ��־����
	 * @param pData ����
	 * @param iDataLen ���ݳ��� 
	 * @param iDataType �������� 
	 */
	void LogInfo(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** ������־��ʾ
	 * @param sLog ��־����
	 * @param pData ����
	 * @param iDataLen ���ݳ��� 
	 * @param iDataType �������� 
	 */
	void LogWarning(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** ������־��ʾ
	 * @param sLog ��־����
	 * @param pData ����
	 * @param iDataLen ���ݳ��� 
	 * @param iDataType �������� 
	 */
	void LogError(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** ����������־��ʾ
	 * @param sLog ��־����
	 * @param pData ����
	 * @param iDataLen ���ݳ��� 
	 * @param iDataType �������� 
	 */
	void LogFatal(LPCSTR sLog, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);


	/** ��־��ʾ
	 * @param sLog ��־����
	 * @param iLogType ��־���� 0:���� 1:��Ϣ 2:���� 3:���� 4:�������� 
	 * @param pData ����
	 * @param iDataLen ���ݳ��� 
	 * @param iDataType �������� 
	 */
	void Log(LPCSTR sLog, int iLogType, const BYTE*pData=NULL, int iDataLen=0, int iDataType=0);

	/** ���غ���
	*/
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);

private:

	/**�ǼǴ���
	*/
	BOOL CLogWnd::RegisterWindowClass();

	/** ����Ŀ¼
	*/
	BOOL CreateMultiLevelDirectory(const CString dd); 

protected:
	//!��������
	CScrollHelper*m_scrollHelper;

	//! save log file
	CStdioFile m_save;

	//! is file open
	BOOL m_bFileOpen;

	//!�������ݵ���
	CRITICAL_SECTION m_csLock;

	//!�Ƿ��б仯
	bool m_bChange;

	//!�Ƿ�������²�
	bool m_bScrollToEnd;

	//!��ʾʱ�������
	int m_iTimeType;

protected:

	//////////////////////////////////////////////////////////////////////////
	//!����
	std::list<CLogInfo*> m_listInfo;

	//�������������ڱ��浱ǰ��Ϣ��������ǰ��Ϣ��֮ǰ��������
	UINT *m_plineCnt;//[MAX_INFO_CNT];

	//!��ǰ��ʾ����������
	INFO_SHOW *m_infoShow;//[MAX_SHOW_CNT];

	//!��ʾ��Ϣ������
	UINT m_iInfoShowCnt;

	//!��ǰ���ѡ���������Ϣ������
	INFO_SHOW m_infoSel;

	//!��Ϣ����
	UINT m_iInfoCnt;

	//!������
	UINT m_iAllLineCnt;

	//!����,��ʾ�е�����
	UINT m_iLineMemSel;

	//��Ϣ����
	unsigned __int64 m_iInfoID;

protected:

	//��ʾ�ĵ�һ������һ��
	UINT m_iFirstShowLineIdx;

	//!��ʾ������
	UINT m_iShowLineCnt;

	//! ʵ�ʿͻ���
	CRect m_rtClient;

	//!���ڴ��е���ʾ���򣬴��ڵ��ڿͻ���
	CRect m_rtMem;

	//!���ڳ�����ʵ����ʾ��λ��X
	int m_iOffsetX;

	//!���ڳ�����ʵ����ʾ��λ��Y
	int m_iOffsetY;

protected:
	//!�и�
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

protected:

	/** draw in dc
	*/
	void OnDraw(CDC*pDC);

	/** draw in memory dc
	*/
	void OnDrawMem(CDC*pDC, CRect& rect);

	/** ������ʾ��������
	*/
	void CalculateLineCnt();

	/** ���������ı���ʾ�߶�
	 */
	void UpdateDisplayHeight();

	/** ������ʾ����
	*/
	void CalculateShowLineCnt();

	/** ��ѡ��ʾ����Ϣ
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
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



