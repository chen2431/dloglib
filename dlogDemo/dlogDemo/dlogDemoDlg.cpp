
// dlogDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "dlogDemo.h"
#include "dlogDemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CScanTh::SingleStep()
{
	if(m_pMainDlg!=NULL)
	{
		return m_pMainDlg->ScanThreadFunction();
	}
	return FALSE;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CdlogDemoDlg �Ի���




CdlogDemoDlg::CdlogDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CdlogDemoDlg::IDD, pParent)
	, m_bInit(FALSE)
	, m_bRun(TRUE)
	, m_pScanThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CdlogDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_LOG, m_log);
}

BEGIN_MESSAGE_MAP(CdlogDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CdlogDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CdlogDemoDlg::OnBnClickedButton2)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CdlogDemoDlg ��Ϣ�������

BOOL CdlogDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
//	CRect rc;//(0,0,500,400);
//	GetDlgItem(IDC_STATIC_LOG)->GetWindowRect(rc);
//	ScreenToClient(rc);

	//m_log.Create(NULL, "CLogWnd", WS_CHILD | WS_VISIBLE |WS_HSCROLL | WS_VSCROLL, rc, this, 1000);
	//m_log.Create(rc, this, IDC_STATIC_LOG);
	//m_log.ModifyStyle(0, WS_CHILD | WS_VISIBLE |WS_HSCROLL | WS_VSCROLL);
	m_log.SetTimeType(1);
	m_log.InitFile("c:\\buaa\\log");

	m_bInit = TRUE;

	SetTimer(1001, 1, NULL);

	StartScanThread();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CdlogDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CdlogDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CdlogDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CdlogDemoDlg::OnBnClickedButton1()
{
	static int c = 0;

	CString s;
	s.Format("data:%d", c);

	BYTE data[8192];
	double*pData = (double*)data;
	for(int i=0; i<1024; i++)
	{
		pData[i] = (double)c + (double)0.01*i;
	}

	c++;

	m_log.LogDataInfo(data, 8192, DATA_DOUBLE, "data");
}


void CdlogDemoDlg::OnBnClickedButton2()
{
	static int i=0;
	CString s;
	s.Format("information:%d", i);

	i++;

	m_log.LogInfo(s, LOG_INFO, NULL, 0);
}


void CdlogDemoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	if(m_bInit)
	{
// 		CRect rect;
// 		GetClientRect(rect);
// 		rect.InflateRect(-10, -10, -10, -100);
// 		GetDlgItem(IDC_STATIC_LOG)->MoveWindow(rect);
// 
// 		CRect rc;
// 		GetDlgItem(IDC_STATIC_LOG)->GetWindowRect(rc);
// 		ScreenToClient(rc);
// 		m_log.MoveWindow(rc);
	}
}


void CdlogDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nIDEvent==1001)
	{
		//OnBnClickedButton1();
	}
	CDialogEx::OnTimer(nIDEvent);
}

/*
*/
BOOL CdlogDemoDlg::ScanThreadFunction()
{
	OnBnClickedButton1();

	Sleep(1);
	return m_bRun;
}

/*! \brief start scan thread
*/
BOOL CdlogDemoDlg::StartScanThread()
{
	ASSERT(m_pScanThread==NULL);

	m_pScanThread=new CScanTh(this);

	if(m_pScanThread==NULL)
	{
		return FALSE;
	}

	if(!m_pScanThread->CreateThread())
	{
		delete m_pScanThread;
		m_pScanThread=NULL;
		return FALSE;
	}

	return TRUE;
}


/*! \brief stop scan thread
*/
void CdlogDemoDlg::StopScanThread()
{
	m_bRun = FALSE;
	if(m_pScanThread!=NULL)
	{
		m_pScanThread->KillThread(FALSE);
		delete m_pScanThread;
		m_pScanThread=NULL;
	}
}


void CdlogDemoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	StopScanThread();
}


void CdlogDemoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnMouseMove(nFlags, point);
}
