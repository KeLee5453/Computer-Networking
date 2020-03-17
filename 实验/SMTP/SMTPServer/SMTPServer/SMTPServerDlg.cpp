
// SMTPServerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SMTPServer.h"
#include "SMTPServerDlg.h"
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
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSMTPServerDlg 对话框



CSMTPServerDlg::CSMTPServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SMTPSERVER_DIALOG, pParent)
	, mc_filename(_T("无附件"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSMTPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_ATTACHMENT, m_picture);
	//  DDX_Control(pDX, IDC_CONTENT, m_content);
	DDX_Control(pDX, IDC_LOG, m_log);
	//  DDX_Control(pDX, IDC_TEXT, m_list);
	//  DDX_Control(pDX, IDC_TEXT, m_text);
	//  DDX_Control(pDX, IDC_ATTACHMENT, m_picture);
	//  DDX_Text(pDX, IDC_NAME, m_filename);
	DDX_Control(pDX, IDC_CONTENT, mc_content);
	DDX_Control(pDX, IDC_TEXT, mc_text);
	DDX_Text(pDX, IDC_NAME, mc_filename);
	DDX_Control(pDX, IDC_SUBJECT, mc_subject);
	//  DDX_Control(pDX, IDC_ATTACHMENT, m_picture);
	DDX_Control(pDX, IDC_ATTACHMENT, m_picture);
}

BEGIN_MESSAGE_MAP(CSMTPServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CLEAR, &CSMTPServerDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_RESTART, &CSMTPServerDlg::OnBnClickedRestart)
END_MESSAGE_MAP()


// CSMTPServerDlg 消息处理程序

BOOL CSMTPServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	
	// TODO: 在此添加额外的初始化代码
	//设置ListBox控件的宽度
	m_log.SetHorizontalExtent(2000);
	mc_content.SetHorizontalExtent(2000);
	mc_text.SetHorizontalExtent(2000);
	mc_subject.SetHorizontalExtent(2000);
	if (m_srvrSocket.m_hSocket == INVALID_SOCKET) {

		//创建监听套接字,激发FD_ACCEPT事件,在TCP的25端口守候,
		BOOL bFlag = m_srvrSocket.Create(25, SOCK_STREAM, FD_ACCEPT);//失败返回-1
		if (!bFlag)//创建失败
		{
			MessageBox(_T("Socket创建失败!"), L"提醒", MB_OK | MB_ICONSTOP);
			m_srvrSocket.Close();
			return TRUE;
		}

		m_log.AddString(_T("***SMTP服务器准备好"));
		m_log.AddString(_T("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"));

		//监听成功,等待连接请求 
		if(!m_srvrSocket.Listen())
		{
			int nErrorCode = m_srvrSocket.GetLastError();    //检测错误信息
			if (nErrorCode != WSAEWOULDBLOCK)             //如果不是线程阻塞
			{
				MessageBox(_T("Socket错误!"), L"提醒", MB_OK | MB_ICONSTOP);
				m_srvrSocket.Close();
				return TRUE;
			}
		}
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSMTPServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSMTPServerDlg::OnPaint()
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
HCURSOR CSMTPServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSMTPServerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_srvrSocket.Close();
	CDialogEx::OnClose();
}


void CSMTPServerDlg::OnBnClickedClear()//清空按钮
{
	// TODO: 在此添加控件通知处理程序代码
	CListBox* plb1 = (CListBox*)GetDlgItem(IDC_CONTENT);//根据ID获取相应的控件
	plb1->ResetContent();//调用ListBox的清空函数。
	CListBox* plb2 = (CListBox*)GetDlgItem(IDC_TEXT);
	plb2->ResetContent();
	CListBox* plb3 = (CListBox*)GetDlgItem(IDC_SUBJECT);
	plb3->ResetContent();
	CStatic* plb4 = (CStatic*)GetDlgItem(IDC_ATTACHMENT);
	plb4->ShowWindow(false);//通过关闭再打开实现清空
	plb4->ShowWindow(true);

	
	
}


void CSMTPServerDlg::OnBnClickedRestart()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(AfxGetMainWnd()->m_hWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
	//获取exe程序当前路径
	extern CSMTPServerApp theApp;
	TCHAR szAppName[MAX_PATH];
	::GetModuleFileName(theApp.m_hInstance, szAppName, MAX_PATH);
	CString strAppFullName;
	strAppFullName.Format(_T("%s"), szAppName);
	//重启程序
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION procStruct;
	memset(&StartInfo, 0, sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);
	::CreateProcess(
		(LPCTSTR)strAppFullName,
		NULL,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&StartInfo,
		&procStruct);

}
