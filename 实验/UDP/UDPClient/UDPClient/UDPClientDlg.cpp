
// UDPClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "UDPClient.h"
#include "UDPClientDlg.h"
#include "afxdialogex.h"
#include "MySocket.h"

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
public:
//	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
//	afx_msg void OnClose();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_CLOSE()
ON_BN_CLICKED(IDCANCEL, &CAboutDlg::OnBnClickedCancel)
//ON_WM_CLOSE()
END_MESSAGE_MAP()


// CUDPClientDlg 对话框



CUDPClientDlg::CUDPClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UDPCLIENT_DIALOG, pParent)
	, m_port(2000)//控件对应的变量
	, m_ip(_T(""))
	, m_reponse(_T(""))
	, m_request(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CUDPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_REQUEST, m_time);
	DDX_Text(pDX, IDC_SERVERPORT, m_port);
	DDX_Text(pDX, IDC_SERVERIP, m_ip);
	DDX_Text(pDX, IDC_REPONSE, m_reponse);
	//  DDX_Text(pDX, IDC_REQUEST, request);
	DDX_Text(pDX, IDC_REQUEST, m_request);
}

BEGIN_MESSAGE_MAP(CUDPClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CUDPClientDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CUDPClientDlg::OnBnClickedOk)
//	ON_WM_CLOSE()
ON_WM_CLOSE()
END_MESSAGE_MAP()


// CUDPClientDlg 消息处理程序

BOOL CUDPClientDlg::OnInitDialog()
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
	if (!AfxSocketInit())//初始化
	{
		MessageBox(L"初始化失败", L"提示", MB_OK | MB_ICONSTOP);
	}
	//产生随机端口
	srand((unsigned)time(NULL));
	int max = RAND_MAX;
	int client_port = (int)(rand() * 5000 / max + 2019);
	if (!m_mysocket.Create(client_port, SOCK_DGRAM, FD_READ))
		//create函数第一个参数是指定的端口号，第二个是连接类型，这里是drame,第三个是监测的事件，这里为读取。
	{
		//以数据报方式创建socket
		MessageBox(L"Socket套接字创建失败", L"错误", MB_OK | MB_ICONSTOP);//一个确定按钮，和一个黄色图标
	}
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUDPClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUDPClientDlg::OnPaint()
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
HCURSOR CUDPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUDPClientDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//清空输入框，这里是建一个指针指向EDIT框，然后设为空字，然后更新数据
	CEdit* pedt = (CEdit*)GetDlgItem(IDC_SERVERIP);
	pedt->SetWindowText(_T(""));
	CEdit* req = (CEdit*)GetDlgItem(IDC_REQUEST);
	req->SetWindowText(_T(""));
	CEdit* rep = (CEdit*)GetDlgItem(IDC_REPONSE);
	rep->SetWindowText(_T(""));
	UpdateData();
}


void CUDPClientDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//先更新数据，然后进行错误处理
	UpdateData(true);
	if(m_ip.IsEmpty())//IP不能为空

	{
		MessageBox(_T("无服务器IP地址！"), _T("错误！"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	if (m_port <= 0 || m_port > 65535)//port在一定范围
	{
		MessageBox(_T("端口值不正确！"), _T("错误！"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	if (m_request.IsEmpty())//请求不能为空
	{
		MessageBox(_T("无请求命令！"), _T("错误！"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	//数据发送到特定目标，未发生错误函数返回发送字符总数，错误返回SOCKET_ERROR，
	//参数：包含数据的缓冲区，数据的长度（以字节为单位），端口号，一个计算机的IP。
	int flag = m_mysocket.SendToEx(m_request.GetBuffer()/*得到指向缓冲区的非常量指针*/, (m_request.GetLength() + 1) * sizeof(WCHAR), m_port, m_ip);
	if (flag == SOCKET_ERROR)
	{
		MessageBox(_T("请求失败！"), _T("错误！"), MB_OK | MB_ICONSTOP);
		return;
	}
}









void CUDPClientDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_mysocket.Close();
	DestroyWindow();
	CDialogEx::OnClose();
}


void CAboutDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


