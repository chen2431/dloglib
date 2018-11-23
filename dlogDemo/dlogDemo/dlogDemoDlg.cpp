
// dlogDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "dlogDemo.h"
#include "dlogDemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CdlogDemoDlg 对话框




CdlogDemoDlg::CdlogDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CdlogDemoDlg::IDD, pParent)
	, m_bInit(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CdlogDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CdlogDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CdlogDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CdlogDemoDlg::OnBnClickedButton2)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CdlogDemoDlg 消息处理程序

BOOL CdlogDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect rc;//(0,0,500,400);
	GetDlgItem(IDC_STATIC_LOG)->GetWindowRect(rc);
	ScreenToClient(rc);

	//m_log.Create(NULL, "CLogWnd", WS_CHILD | WS_VISIBLE |WS_HSCROLL | WS_VSCROLL, rc, this, 1000);
	m_log.Create(rc, this, IDC_STATIC_LOG);
	m_log.SetTimeType(1);
	m_log.InitFile("c:\\buaa\\log");

	m_bInit = TRUE;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CdlogDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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

	// TODO: 在此处添加消息处理程序代码
	if(m_bInit)
	{
		CRect rect;
		GetClientRect(rect);
		rect.InflateRect(-10, -10, -10, -100);
		GetDlgItem(IDC_STATIC_LOG)->MoveWindow(rect);

		CRect rc;
		GetDlgItem(IDC_STATIC_LOG)->GetWindowRect(rc);
		ScreenToClient(rc);
		m_log.MoveWindow(rc);
	}
}
