
// UDPserverDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "UDPserver.h"
#include "UDPserverDlg.h"
#include "afxdialogex.h"
#include"MySocket.h"
#include<WS2tcpip.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS


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


// CUDPserverDlg 对话框



CUDPserverDlg::CUDPserverDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UDPSERVER_DIALOG, pParent)
	, m_date(_T(""))
	, m_port(2000)
	, m_time(_T(""))
	, m_ip(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUDPserverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DATE, m_date);
	//  DDX_CBString(pDX, IDC_IP, m_ip);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Text(pDX, IDC_TIME, m_time);
	//  DDX_Text(pDX, IDC_LOG, m_log);
	//  DDX_Control(pDX, IDC_LOG, m_log);
	DDX_Control(pDX, IDC_LOG, m_log);
	//  DDX_Control(pDX, IDC_IP, m_ip);
	DDX_Control(pDX, IDC_IP, m_conip);
	DDX_CBString(pDX, IDC_IP, m_ip);
}

BEGIN_MESSAGE_MAP(CUDPserverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_EN_CHANGE(IDC_EDIT2, &CUDPserverDlg::OnEnChangeEdit2)
ON_WM_CLOSE()
END_MESSAGE_MAP()


// CUDPserverDlg 消息处理程序

BOOL CUDPserverDlg::OnInitDialog()
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
	//获取主机名与可用IP地址
	update_date_and_time(m_date, m_time);//获取当前日期和时间放入。



	if (!AfxSocketInit()) {//初始化

		MessageBox(L"初始化失败", L"提示", MB_OK | MB_ICONSTOP);

	}

	update_date_and_time(m_date, m_time);//获取当前日期和时间

	m_log.AddString(m_date + _T(" ") + m_time + _T(":") + _T("Socket初始化成功！"));
	//listbox添加记录

	char hostname[20] = "";
	int errorcode;
	if ((errorcode = gethostname(hostname, sizeof(hostname))) != 0)//通过获取主机名
	{
		char char_error[100];
		_itoa_s(errorcode, char_error, 10);//转化为字符串
		MessageBoxA(this->GetSafeHwnd(), char_error, "errorcode", MB_OK);//获取对话框的句柄然后输出错误信息
		AfxGetMainWnd()->SendMessage(WM_CLOSE);//程序退出

	}
	CString hostname_cstring(hostname);//把char*转化为CString
	CString record;
	record.Format(L"当前获取的主机名是：【%s】", hostname_cstring);//记录格式化
	update_date_and_time(m_date, m_time);
	m_log.AddString(m_date + _T("  " + m_time + _T("：") + record));//记录时间

	hostent* hn;

	hn = gethostbyname(hostname);//通过主机名获取主机信息


	/*
	返回hostent结构体类型指针
    struct hostent
    {
        char    *h_name;               
        char    **h_aliases;
        int     h_addrtype;
        int     h_length;
        char    **h_addr_list;
        #define h_addr h_addr_list[0]
    };

    hostent->h_name
    表示的是主机的规范名。例如www.google.com的规范名其实是www.l.google.com。
    
    hostent->h_aliases
    表示的是主机的别名.www.google.com就是google他自己的别名。有的时候，有的主机可能有好几个别名，这些，其实都是为了易于用户记忆而为自己的网站多取的名字。

    hostent->h_addrtype    
    表示的是主机ip地址的类型，到底是ipv4(AF_INET)，还是pv6(AF_INET6)

    hostent->h_length      
    表示的是主机ip地址的长度

    hostent->h_addr_lisst
    表示的是主机的ip地址，注意，这个是以网络字节序存储的。千万不要直接用printf带%s参数来打这个东西，会有问题的哇。所以到真正需要打印出这个IP的话，需要调用inet_ntop()。

    const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt) ：
    这个函数，是将类型为af的网络地址结构src，转换成主机序的字符串形式，存放在长度为cnt的字符串中。返回指向dst的一个指针。如果函数调用错误，返回值是NULL。
	*/
	int i = 0;

	while (hn->h_addr_list[i] != 0) {

		char* p = inet_ntoa(*(in_addr*)hn->h_addr_list[i++]);

		wchar_t pw[20];

		SHAnsiToUnicode(p, pw, 20);

		CString str;

		str.Format(L"%s", pw);//转化为CSTRING

		m_conip.AddString(str);

		record.Format(L"当前主机第 %d 个可用IP地址是： 【 %s 】", i, pw);

		update_date_and_time(m_date, m_time);//更新日期和时间

		m_log.AddString(m_date + _T(" ") + m_time + _T(":") + record);//增加记录

	}

	if (i == 0) {

		AfxGetMainWnd()->SendMessage(WM_CLOSE);
		//关闭程序
	}

	UpdateData(false);

	m_conip.SetCurSel(0);//强制选择第一个项
	UpdateData(true);

	//启动服务器操作



	if (m_ip.IsEmpty())

	{
		MessageBox(_T("无服务器Ip地址！"), _T("错误！"), MB_OK | MB_ICONEXCLAMATION); return TRUE;
	}

	if (m_port <= 0 || m_port > 65535)

	{
		MessageBox(_T("端口值设置错误！"), _T("错误！"), MB_OK | MB_ICONEXCLAMATION); return TRUE;
	}



	UpdateData(true);

	if (!m_mysocket.Create(m_port, SOCK_DGRAM, FD_READ, m_ip)) {//以数据报方式创建socket

		MessageBox(L"Socket套接字创建失败", L"错误", MB_OK | MB_ICONSTOP); return TRUE;

	};

	record.Format(L"启动服务器成功！当前主机IP是：【 %s 】，端口号是：【 %u 】！", m_ip, m_port);

	update_date_and_time(m_date, m_time);//更新日期和时间

	m_log.AddString(m_date + _T(" ") + m_time + _T(":") + record);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUDPserverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUDPserverDlg::OnPaint()
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
HCURSOR CUDPserverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CUDPserverDlg::update_date_and_time(CString& date, CString& time)//获取当前的信息
{
	date = CTime::GetCurrentTime().Format(L"%Y/%m/%d");

	time = CTime::GetCurrentTime().Format("%H:%M:%S");

	UpdateData(false);// TODO: 在此处添加实现代码.
}


void CUDPserverDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_mysocket.Close();
	CDialogEx::OnClose();
}
