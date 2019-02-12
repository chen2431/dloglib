
// dlogDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "LogWnd.h"
#include "ThreadBase.h"

class CdlogDemoDlg;
/** �߳������ڸ�������*/
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


// CdlogDemoDlg �Ի���
class CdlogDemoDlg : public CDialogEx
{
// ����
public:
	CdlogDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

	CLogWnd m_log;
	BOOL m_bInit;


	//ɨ���߳�
	BOOL ScanThreadFunction();
	//�����߳�
	BOOL StartScanThread();
	//ֹͣ�߳�
	void StopScanThread();
	CScanTh *m_pScanThread;	//!<�߳�
	//!�߳�����
	BOOL m_bRun;

// �Ի�������
	enum { IDD = IDD_DLOGDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��





// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
