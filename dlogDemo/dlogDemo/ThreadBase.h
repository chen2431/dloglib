//////////////////////////////////////////////////////////////////////////
// 这个类是参考MSDN的例子MTGDI写的
// 对CWinThread进行了一次封装，增加了线程结束的机制
// 目的是比较方便的管理线程的创建、运行和退出
//
//  使用时将这个类作为基类，并重载CreateThread函数和InitInstance函数
//
//  在重载CreateThread函数时
//  在CreateThread参数中传入线程函数指针或包含线程函数作为成员函数的类的指针
//  再调用CWinThread::CreateThread
//	
//	在重载InitInstance函数时，使用KeepRunning()作为线程函数运行的条件，如:
//	
//	BOOL InitInstance()
//	{
//		...
//		while(KeepRunning())
//		{
//			//调用线程函数
//			
//		}
//		
//		return FALSE;
//	}
//	注意：重载的InitInstance最后要返回FALSE
//	与经常使用的线程函数不同的是：这里的线程函数将在线程运行期间不断被调用
//	因此只用实现一个周期的操作；传统的线程函数是指在函数内部一直循环，直到线
//	程结束后才返回的那种
//
//	当需要结束线程时，调用KillThread()成员函数
//	KillThread()将无限期等待线程函数和InitInstance返回，以确保线程可靠结束
//	
//	另外，还需注意，使用KillThread将线程结束后，类的实例已经删除
//	如果使用new创建实例，不能再执行Delete操作
//
//	如果派生类有分配的内存需要删除，必要时可重载Delete函数
//
//	陆国雷
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

	virtual BOOL SingleStep(){return FALSE;}	//必须重载,返回FALSE线程也退出，但不会自动删除
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
	HANDLE m_hEventKill;	//退出线程事件
	HANDLE m_hEventDead;	//线程已退出事件

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
