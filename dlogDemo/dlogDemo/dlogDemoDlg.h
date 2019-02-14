
// dlogDemoDlg.h : 头文件
//

#pragma once
#include "LogWnd.h"
#include "ThreadBase.h"

class CdlogDemoDlg;
/** 线程类用于更新数据*/
class CScanTh : public CThreadBase
{
public:
	CScanTh(CdlogDemoDlg *pMainDlg){m_pMainDlg = pMainDlg;}
	virtual ~CScanTh(){;}
	//! step function
	virtual BOOL SingleStep();

protected:
	//! the host's pointer
	CdlogDemoDlg *m_pMainDlg;
};


// CdlogDemoDlg 对话框
class CdlogDemoDlg : public CDialogEx
{
// 构造
public:
	CdlogDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

	CLogWnd m_log;
	BOOL m_bInit;


	//扫描线程
	BOOL ScanThreadFunction();
	//启动线程
	BOOL StartScanThread();
	//停止线程
	void StopScanThread();
	CScanTh *m_pScanThread;	//!<线程
	//!线程运行
	BOOL m_bRun;

// 对话框数据
	enum { IDD = IDD_DLOGDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持





// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
