
// dlogDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "LogWnd.h"

// CdlogDemoDlg �Ի���
class CdlogDemoDlg : public CDialogEx
{
// ����
public:
	CdlogDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

	CLogWnd m_log;
	BOOL m_bInit;

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
};
