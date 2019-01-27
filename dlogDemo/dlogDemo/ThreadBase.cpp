// ThreadBase.cpp : implementation file
//

#include "stdafx.h"
#include "ThreadBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThreadBase

IMPLEMENT_DYNCREATE(CThreadBase, CWinThread)

CThreadBase::CThreadBase()
{
	m_bAutoDelete=FALSE;
	m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CThreadBase::~CThreadBase()
{
	CloseHandle(m_hEventKill);
	CloseHandle(m_hEventDead);
}

BOOL CThreadBase::InitInstance()
{
	BOOL bKeepRunning=TRUE;
	while(KeepRunning() && bKeepRunning)
	{
		bKeepRunning=SingleStep();
	}

	return FALSE;
}

void CThreadBase::Delete()
{
	//InitInstance�������غ�ϵͳ�����AfxEndThread����
	//AfxEndThread���������thread��Delete��Ա
	//����߳��˳���������������ã�ʹm_hEventDead������
	//
	CWinThread::Delete();

	VERIFY(SetEvent(m_hEventDead));
}

void CThreadBase::SendKillMessage()
{
	VERIFY(SetEvent(m_hEventKill));
}


void CThreadBase::KillThread(BOOL bDelete)
{
	VERIFY(SetEvent(m_hEventKill));

	// allow thread to run at higher priority during kill process
	SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	Sleep(10);

	BOOL bTimeout=FALSE;

	WaitForSingleObject(m_hEventDead, INFINITE);
		
	WaitForSingleObject(m_hThread, INFINITE);
		

	if(bDelete)
		delete this;
}

int CThreadBase::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();

}

BEGIN_MESSAGE_MAP(CThreadBase, CWinThread)
	//{{AFX_MSG_MAP(CThreadBase)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThreadBase message handlers
