//////////////////////////////////////////////////////////////////////////
// ������ǲο�MSDN������MTGDIд��
// ��CWinThread������һ�η�װ���������߳̽����Ļ���
// Ŀ���ǱȽϷ���Ĺ����̵߳Ĵ��������к��˳�
//
//  ʹ��ʱ���������Ϊ���࣬������CreateThread������InitInstance����
//
//  ������CreateThread����ʱ
//  ��CreateThread�����д����̺߳���ָ�������̺߳�����Ϊ��Ա���������ָ��
//  �ٵ���CWinThread::CreateThread
//	
//	������InitInstance����ʱ��ʹ��KeepRunning()��Ϊ�̺߳������е���������:
//	
//	BOOL InitInstance()
//	{
//		...
//		while(KeepRunning())
//		{
//			//�����̺߳���
//			
//		}
//		
//		return FALSE;
//	}
//	ע�⣺���ص�InitInstance���Ҫ����FALSE
//	�뾭��ʹ�õ��̺߳�����ͬ���ǣ�������̺߳��������߳������ڼ䲻�ϱ�����
//	���ֻ��ʵ��һ�����ڵĲ�������ͳ���̺߳�����ָ�ں����ڲ�һֱѭ����ֱ����
//	�̽�����ŷ��ص�����
//
//	����Ҫ�����߳�ʱ������KillThread()��Ա����
//	KillThread()�������ڵȴ��̺߳�����InitInstance���أ���ȷ���߳̿ɿ�����
//	
//	���⣬����ע�⣬ʹ��KillThread���߳̽��������ʵ���Ѿ�ɾ��
//	���ʹ��new����ʵ����������ִ��Delete����
//
//	����������з�����ڴ���Ҫɾ������Ҫʱ������Delete����
//
//	½����
//	2007-10-07
//
#if !defined(AFX_THREADBASE_H__82945B37_59DC_43C7_A257_11B621A605FF__INCLUDED_)
#define AFX_THREADBASE_H__82945B37_59DC_43C7_A257_11B621A605FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// CThreadBase thread

class CThreadBase : public CWinThread
{
public:
	DECLARE_DYNCREATE(CThreadBase)
	CThreadBase();           // protected constructor used by dynamic creation
	
// Attributes
public:
	
// Operations
public:
	void SendKillMessage();
	void KillThread(BOOL bDelete=TRUE);	
	
	BOOL KeepRunning(){return WAIT_TIMEOUT==WaitForSingleObject(m_hEventKill,0);}

	virtual BOOL SingleStep(){return FALSE;}	//��������,����FALSE�߳�Ҳ�˳����������Զ�ɾ��
// Implementation
public:
	virtual void Delete();
	virtual ~CThreadBase();

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThreadBase)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HANDLE m_hEventKill;	//�˳��߳��¼�
	HANDLE m_hEventDead;	//�߳����˳��¼�

	// Generated message map functions
	//{{AFX_MSG(CThreadBase)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THREADBASE_H__82945B37_59DC_43C7_A257_11B621A605FF__INCLUDED_)
