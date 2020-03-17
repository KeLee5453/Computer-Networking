
// FTPDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FTP.h"
#include "FTPDlg.h"
#include "afxdialogex.h"
using namespace std;
#undef UNICODE
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//界面文件索引
int IndexList;

//客户端打开的文件句柄
CFile file;
CFile filedownload;
SOCKADDR_IN addrFromC;
//服务器文件列表
int mapindex = 1;
map<int, CString> map_filelist;

//服务器写入文件的句柄,用来提供多客户端同时上传文件
map<SOCKADDR_IN*, CFile*> map_readfile;
map<SOCKADDR_IN*, CFile*> map_writfile;

//客户端文件列表
CString list_filelist[MTU] = {_T("") };

SOCKET m_socket;

//获取程序当前的路径
CString GetExePath()
{
	//获取exe程序当前路径
	extern CFTPApp theApp;
	TCHAR szAppName[MAX_PATH];
	::GetModuleFileName(theApp.m_hInstance, szAppName, MAX_PATH);
	CString strAppFullName;
	strAppFullName.Format(_T("%s"), szAppName);
	int pos = strAppFullName.ReverseFind('\\');
	strAppFullName = strAppFullName.Left(pos + 1);
	return strAppFullName;
}
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
//	afx_msg LRESULT OnRecvpro(WPARAM wParam, LPARAM lParam);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_MESSAGE(RecvPro, &CAboutDlg::OnRecvpro)
END_MESSAGE_MAP()


// CFTPDlg 对话框



CFTPDlg::CFTPDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FTP_DIALOG, pParent)
	, m_radio(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFTPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_RADIO_SER, m_radio);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipdress);
	DDX_Control(pDX, IDC_LIST_FILE, m_filelist);
	//  DDX_Radio(pDX, IDC_RADIO_SER, m_radio);
	DDX_Control(pDX, IDC_LOG, m_log);
	DDX_Radio(pDX, IDC_RADIO_SER, m_radio);
}

BEGIN_MESSAGE_MAP(CFTPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &CFTPDlg::OnClickedButtonRestart)
	ON_BN_CLICKED(IDC_BUTTON_START, &CFTPDlg::OnClickedButtonStart)
	ON_BN_CLICKED(IDC_RADIO_SER, &CFTPDlg::OnClickedRadioSer)
	ON_COMMAND(IDC_RADIO_CLI, &CFTPDlg::OnRadioCli)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CFTPDlg::OnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_SEL_FILE, &CFTPDlg::OnClickedButtonSelFile)
	ON_BN_CLICKED(IDC_BUTTON_VIEWFILE, &CFTPDlg::OnClickedButtonViewfile)
	ON_BN_CLICKED(IDC_BUTTON_DOWLFILE, &CFTPDlg::OnClickedButtonDowlfile)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILE, &CFTPDlg::OnClickListFile)
END_MESSAGE_MAP()


// CFTPDlg 消息处理程序

BOOL CFTPDlg::OnInitDialog()
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



	//ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
	m_log.SetHorizontalExtent(500);
	initlistctrl();
	InitSocket();

	IndexList = -1;

	GetDlgItem(IDC_BUTTON_SEL_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_VIEWFILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DOWLFILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFTPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFTPDlg::OnPaint()
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
HCURSOR CFTPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFTPDlg::OnClickedButtonRestart()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(AfxGetMainWnd()->m_hWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
	//获取exe程序当前路径
	extern CFTPApp theApp;
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


// 初始化文件列表
void CFTPDlg::initlistctrl()
{
	// TODO: 在此处添加实现代码.
	LONG lStyle;
	lStyle = GetWindowLong(m_filelist.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_filelist.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = m_filelist.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	m_filelist.SetExtendedStyle(dwStyle); //设置扩展风格
	m_filelist.InsertColumn(0, _T("文件名"), LVCFMT_LEFT, 315);//插入列
}



// 创建套接字
bool CFTPDlg::InitSocket()
{
	// TODO: 在此处添加实现代码.
	m_socket=socket(AF_INET,SOCK_DGRAM,0);
	if(INVALID_SOCKET==m_socket)
	{
		MessageBox(_T("套接字创建失败！"));
		return FALSE;
	}
	return TRUE;
}


// 绑定套接字端口
void CFTPDlg::BindSocket()
{
	// TODO: 在此处添加实现代码.
	if (m_radio == 0)
	{
		SOCKADDR_IN addrSock;//创建SOCKADDR_IN结构体变量
		addrSock.sin_family = AF_INET;//使用IPV4地址
		addrSock.sin_port = htons(6000);//端口号
		addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//IP地址

		int retval;
		retval = bind(m_socket, (SOCKADDR*)&addrSock, sizeof(SOCKADDR));//绑定
		if (SOCKET_ERROR == retval)
		{
			closesocket(m_socket);
			MessageBox(_T("绑定失败!"));
			return;
		}
	}

	return;
}


void CFTPDlg::OnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_radio == 0)//如果是服务器就绑定端口号
	{
		BindSocket();
		getFileList(GetExePath());

		m_log.AddString(_T("服务器已启动！"));
		int nCount = m_log.GetCount();
		if (nCount > 0)
			m_log.SetCurSel(nCount - 1);
		GetDlgItem(IDC_RADIO_SER)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_CLI)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_SEL_FILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_VIEWFILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWLFILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
		/*GetDlgItem(IDC_STATIC)->EnableWindow(FALSE);
	
		GetDlgItem(IDC_EDIT_MESSAGE)->EnableWindow(FALSE);*/
		
	}
	else
	{
		if (m_ipdress.IsBlank())
		{
			MessageBox(_T("请先填入服务器IP地址"));
			return;
		}
		DWORD dwIP;
		((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);//获取填入的IP地址

		SOCKADDR_IN addrTo;
		addrTo.sin_family = AF_INET;
		addrTo.sin_port = htons(6000);
		addrTo.sin_addr.S_un.S_addr = htonl(dwIP);

		Packet FileData;
		FileData.flag = 0;
		strcpy((char*)FileData.Data, "请求登录服务器");

		sendto(m_socket, (char*)&FileData, sizeof(FileData), 0, (SOCKADDR*)&addrTo, sizeof(SOCKADDR));//发送给服务器
		char tempBuf[MTU * 2];
		sprintf(tempBuf, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrTo.sin_addr), FileData.Data);
		CString strt(tempBuf);


		m_log.AddString(strt);
		int nCount = m_log.GetCount();
		if (nCount > 0)
			m_log.SetCurSel(nCount - 1);
		//设置按键
		GetDlgItem(IDC_RADIO_SER)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_CLI)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_SEL_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_VIEWFILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DOWLFILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		UpdateData(FALSE);
		
		
	}

	HANDLE hThread = CreateThread(NULL, 0, RecvProc, this, 0, NULL);//创建接收消息线程
	CloseHandle(hThread);
	
	
}
//接收消息线程
DWORD WINAPI CFTPDlg::RecvProc(LPVOID lpParameter)
{
	CFTPDlg* p_dlg = (CFTPDlg*)lpParameter;

	SOCKET sock = m_socket;

	SOCKADDR_IN addrFrom;
	int len = sizeof(SOCKADDR);

	Packet FileData;
	int retval;
	char tempBuf[MTU * 2];

	while (TRUE)//一直循环对话
	{
		retval = recvfrom(sock, (char*)&FileData, sizeof(FileData), 0, (SOCKADDR*)&addrFrom, &len);
		addrFromC = addrFrom;
		if (SOCKET_ERROR == retval)
			break;
		if ( FileData.flag == 2||FileData.flag==-5)
		{
			wsprintfA(tempBuf, "收到IP为：%s的消息。\r\n", inet_ntoa(addrFrom.sin_addr));
		}
		else
		{
			sprintf(tempBuf, "收到IP为：%s的消息:\r\n%s", inet_ntoa(addrFrom.sin_addr), FileData.Data);
		}
		CString strt(tempBuf);
		p_dlg->m_log.AddString(strt);
		
		//服务器收到：0代表从客户端第一次发来消息
		if (FileData.flag == 0)
		{
			//看该客户端的文件指针存不存在
			bool isexist = false;
			for (auto it = map_writfile.begin(); it != map_writfile.end(); ++it)
			{
				if (it->first->sin_family == addrFrom.sin_family && it->first->sin_port == addrFrom.sin_port && it->first->sin_addr.S_un.S_addr == addrFrom.sin_addr.S_un.S_addr)
				{
					isexist = true;
				}
			}
			if (isexist == false)//如果不存在就创建
			{
				SOCKADDR_IN* addrclient = new SOCKADDR_IN;
				*addrclient = addrFrom;
				map_writfile[addrclient] = new CFile;
				map_readfile[addrclient] = new CFile;
			}
		}
		//服务器收到：客户端选择了“选择按钮”即将要给服务器发送消息，并把flag置为-1
		if (FileData.flag == 1)
		{
			//服务端接收到了文件名
			CFile* filew = NULL;
			for (auto it = map_writfile.begin(); it != map_writfile.end(); ++it)
			{
				if (it->first->sin_family == addrFrom.sin_family && it->first->sin_port == addrFrom.sin_port && it->first->sin_addr.S_un.S_addr == addrFrom.sin_addr.S_un.S_addr)
				{
					filew = it->second;
				}
			}

			//获取程序当前运行的路径，并把接收的文件存放在当前路径下
			FileData.initdata();
			CString mfilepath = GetExePath();
			mfilepath += FileData.filename;

			//把接收的文件名存入服务器列表中
			map_filelist[mapindex++] = FileData.filename;

			//服务器创建新的文件
			filew->Open(mfilepath, CFile::modeCreate | CFile::modeWrite);
			Packet FileDatatemp = FileData;

			//标志设置为-1允许接收数据
			FileDatatemp.flag = -1;

			CString temptex(FileData.filename) ;
			CString temptex1(FileData.filename);
			temptex.Format(_T("%s %s"), _T("服务器允许接收文件:"), temptex1);

			//strcpy((char*)FileDatatemp.Data, (char*)temptex.GetBuffer());
			char tempbuf[MTU * 2] = {0};
			wsprintfA(tempbuf, "%ls", temptex);
			strcpy((char*)FileData.Data, tempbuf);
			sendto(sock, (char*)&FileDatatemp, sizeof(FileDatatemp), 0, (SOCKADDR*)&addrFrom, sizeof(SOCKADDR));
			p_dlg->m_filelist.InsertItem(p_dlg->m_filelist.GetItemCount() , _T(""));//插入行
			p_dlg->m_filelist.SetItemText(p_dlg->m_filelist.GetItemCount()-1 , 0, temptex1);//设置数据
			sprintf(tempBuf, "向IP为：%s 发送消息:\r\n %s", inet_ntoa(addrFrom.sin_addr), tempbuf);
			CString strt(tempBuf);
			p_dlg->m_log.AddString(strt);
			int nCount = p_dlg->m_log.GetCount();
			if (nCount > 0)
				p_dlg->m_log.SetCurSel(nCount - 1);
			continue;
		}
		//客户端收到：服务端允许上传文件数据断并把flag置为2让服务器写入数据
		if (FileData.flag == -1)
		{
			//最后一个包，如果数据不满MTU的处理
			FileData.initdata();
			if (FileData.Number == FileData.Total)
			{
				file.Read(FileData.Data, FileData.Totallength - FileData.Number * MTU);
				FileData.length = FileData.Totallength - FileData.Number * MTU;
			}
			else
			{
				file.Read(FileData.Data, MTU);
				FileData.length = MTU;
			}
			//flag=2，让对方写入
			Packet FileDatatemp = FileData;
			FileDatatemp.flag = 2;
			sendto(sock, (char*)&FileDatatemp, sizeof(FileDatatemp), 0, (SOCKADDR*)&addrFrom, sizeof(SOCKADDR));

			wsprintfA(tempBuf, "向IP为：%s发送消息.\r\n", inet_ntoa(addrFrom.sin_addr));
			CString strt(tempBuf);
			p_dlg->m_log.AddString(strt);

			//如果包的数量等于总数量则传送完毕
			if (FileData.Number == FileData.Total)
			{
				file.Close();
				FileData.Number = 0;//初始化

				p_dlg->m_log.AddString(_T("传送完成!"));
				int nCount = p_dlg->m_log.GetCount();
				if (nCount > 0)
					p_dlg->m_log.SetCurSel(nCount - 1);
			}
			continue;
		}
		//服务器收到：客户端发来的消息即服务器需要写入文件，并把flag置为-1让客户端继续发送
		if (FileData.flag == 2)
		{
			CFile* filew = NULL;
			for (auto it = map_writfile.begin(); it != map_writfile.end(); ++it)
			{
				if (it->first->sin_family == addrFrom.sin_family && it->first->sin_port == addrFrom.sin_port && it->first->sin_addr.S_un.S_addr == addrFrom.sin_addr.S_un.S_addr)
				{
					filew = it->second;
				}
			}

			//服务端收到客户端上传的文件数据段
			filew->Write(FileData.Data, FileData.length);

			if (FileData.Number == FileData.Total)
			{
				filew->Close();
				FileData.Number = 0;//初始化

				p_dlg->m_log.AddString(_T("接收完成!"));
				int nCount = p_dlg->m_log.GetCount();
				if (nCount > 0)
					p_dlg->m_log.SetCurSel(nCount - 1);
				
				continue;
			}
			FileData.initdata();
			FileData.Number++;//把文件的数量加一为了看有没有接收完毕
			Packet FileDatatemp = FileData;
			FileDatatemp.flag = -1;


			strcpy((char*)FileDatatemp.Data, "服务器允许继续上传文件段\n");

			sendto(sock, (char*)&FileDatatemp, sizeof(FileDatatemp), 0, (SOCKADDR*)&addrFrom, sizeof(SOCKADDR));

			sprintf(tempBuf, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrFrom.sin_addr), FileDatatemp.Data);
			CString strt(tempBuf);
			p_dlg->m_log.AddString(strt);
			continue;
		}
		//服务端收到文件列表请求并把flag置为-3
		if (FileData.flag ==3)
		{
			FileData.initdata();
			Packet FileDatatemp = FileData;
			FileDatatemp.initfilelist();
			int mindex = 0;
			//把文件名称写入包里的文件列表
			for (auto it = map_filelist.begin(); it != map_filelist.end(); ++it)
			{
				wsprintfA(FileDatatemp.mfilelist[mindex++].filename, "%ls", it->second);

				if (mindex == FILENUM)
				{

					p_dlg->m_log.AddString(_T("文件超出个数!"));
					int nCount = p_dlg->m_log.GetCount();
					if (nCount > 0)
						p_dlg->m_log.SetCurSel(nCount - 1);
					break;
				}
			}
			FileDatatemp.flag = -3;
			strcpy((char*)FileDatatemp.Data, "服务器下传文件列表");
			sendto(sock, (char*)&FileDatatemp, sizeof(FileDatatemp), 0, (SOCKADDR*)&addrFrom, sizeof(SOCKADDR));

			sprintf(tempBuf, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrFrom.sin_addr), FileDatatemp.Data);
			CString strt(tempBuf);

			p_dlg->m_log.AddString(strt);
			int nCount = p_dlg->m_log.GetCount();
			if (nCount > 0)
				p_dlg->m_log.SetCurSel(nCount - 1);
			continue;
		}
		//客户端收到：文件列表回复
		if (FileData.flag == -3)
		{
			FileData.initdata();
			//先初始化本地列表
			for (int mindex = 0; mindex < MTU; mindex++)
			{
				list_filelist[mindex] = "";
			}

			//把服务器的文件列表复制到本地
			for (int mindex = 0; mindex < FILENUM; mindex++)
			{
				list_filelist[mindex] = FileData.mfilelist[mindex].filename;
			}
			p_dlg->insertlistctrl();

			continue;
		}

		if (FileData.flag == 4)
		{
			CFile* filedownload = NULL;
			for (auto it = map_readfile.begin(); it != map_readfile.end(); ++it)
			{
				if (it->first->sin_family == addrFrom.sin_family && it->first->sin_port == addrFrom.sin_port && it->first->sin_addr.S_un.S_addr == addrFrom.sin_addr.S_un.S_addr)
				{
					filedownload = it->second;
				}
			}

			//服务器收到下载请求,打开待下载文件
			CString mfilepath = GetExePath();
			mfilepath += FileData.filename;
			CString tempfilename(FileData.filename);
			//服务器打开待下载的文件
			filedownload->Open(mfilepath, CFile::modeRead);

			long downloadFileLength = filedownload->GetLength();//获取文件的长度

			FileData.init();
			wsprintfA(FileData.filename, "%ls", tempfilename);
			//计算包的总数
			FileData.Total = downloadFileLength / MTU - 1;
			if (downloadFileLength % MTU != 0)
			{
				FileData.Total = downloadFileLength / MTU;
			}
			FileData.Number = 0;
			FileData.flag = -4;
			FileData.length = downloadFileLength;
			FileData.Totallength = downloadFileLength;

			strcpy((char*)FileData.Data, "服务器准备下传文件");
			sprintf(tempBuf, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrFrom.sin_addr), FileData.Data);

			sendto(m_socket, (char*)&FileData, sizeof(FileData), 0, (SOCKADDR*)&addrFrom, sizeof(SOCKADDR));
			CString strt(tempBuf);

			p_dlg->m_log.AddString(strt);
			int nCount = p_dlg->m_log.GetCount();
			if (nCount > 0)
				p_dlg->m_log.SetCurSel(nCount - 1);
			continue;
		}

		if (FileData.flag == -4)
		{
			//客户端收到下载请求的回复
			//获取程序当前运行的路径，并把接收的文件存放在当前路径下
			FileData.initdata();
			CString mfilepath = GetExePath();
			mfilepath += FileData.filename;

			//客户端创建新的文件
			filedownload.Open(mfilepath, CFile::modeCreate | CFile::modeWrite);
			Packet FileDatatemp = FileData;

			//标志设置为5准备开始接收数据
			FileDatatemp.flag = 5;

			CString temptex (FileDatatemp.filename);
			temptex.Format(_T("%s%s"), _T("客户端准备接收文件完毕: "), temptex);
			char tempbuf[MTU * 2] = { 0 };
			wsprintfA(tempbuf, "%ls", temptex);
			strcpy((char*)FileData.Data, tempbuf);
			sendto(sock, (char*)&FileDatatemp, sizeof(FileDatatemp), 0, (SOCKADDR*)&addrFrom, sizeof(SOCKADDR));

			sprintf(tempBuf, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrFrom.sin_addr), tempbuf);
			CString strt(tempBuf);

			p_dlg->m_log.AddString(strt);
			int nCount = p_dlg->m_log.GetCount();
			if (nCount > 0)
				p_dlg->m_log.SetCurSel(nCount - 1);

			continue;
		}

		if (FileData.flag == 5)
		{
			//服务器开始发送文件段到客户端
			CFile* filedownload = NULL;
			for (auto it = map_readfile.begin(); it != map_readfile.end(); ++it)
			{
				if (it->first->sin_family == addrFrom.sin_family && it->first->sin_port == addrFrom.sin_port && it->first->sin_addr.S_un.S_addr == addrFrom.sin_addr.S_un.S_addr)
				{
					filedownload = it->second;
				}
			}

			FileData.initdata();
			if (FileData.Number == FileData.Total)
			{
				filedownload->Read(FileData.Data, FileData.Totallength - FileData.Number * MTU);
				FileData.length = FileData.Totallength - FileData.Number * MTU;
			}
			else
			{
				filedownload->Read(FileData.Data, MTU);
				FileData.length = MTU;
			}
			//flag=-5，让客户端写入
			Packet FileDatatemp = FileData;
			FileDatatemp.flag = -5;
			sendto(sock, (char*)&FileDatatemp, sizeof(FileDatatemp), 0, (SOCKADDR*)&addrFrom, sizeof(SOCKADDR));

			wsprintfA(tempBuf, "向IP为：%s发送消息。\r\n", inet_ntoa(addrFrom.sin_addr));
			CString strt(tempBuf);
			p_dlg->m_log.AddString(strt);

			if (FileData.Number == FileData.Total)
			{
				filedownload->Close();
				FileData.Number = 0;//初始化
				
				p_dlg->m_log.AddString(_T("传送完毕！"));
				int nCount = p_dlg->m_log.GetCount();
				if (nCount > 0)
					p_dlg->m_log.SetCurSel(nCount - 1);
			}
			continue;
		}

		if (FileData.flag == -5)
		{
			//客户端接收服务器发送的文件段
			filedownload.Write(FileData.Data, FileData.length);
			if (FileData.Number == FileData.Total)
			{
				filedownload.Close();
				FileData.Number = 0;//初始化

				p_dlg->m_log.AddString(_T("接收完毕！"));
				int nCount = p_dlg->m_log.GetCount();
				if (nCount > 0)
					p_dlg->m_log.SetCurSel(nCount - 1);
				continue;
			}
			FileData.initdata();
			FileData.Number++;
			Packet FileDatatemp = FileData;
			FileDatatemp.flag = 5;


			strcpy((char*)FileDatatemp.Data, "客户端请求继续下载文件段");

			sendto(sock, (char*)&FileDatatemp, sizeof(FileDatatemp), 0, (SOCKADDR*)&addrFrom, sizeof(SOCKADDR));

			sprintf(tempBuf, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrFrom.sin_addr), FileDatatemp.Data);
			CString strt(tempBuf);
			p_dlg->m_log.AddString(strt);
			continue;
		}

		if (FileData.flag == 6)
		{
			//此为收到其他客户端或者服务端的消息。因为上面recvform已经打印消息到界面。此处不需要再打印
			FileData.initdata();
		
			continue;
		}
	}
	return 0;
}


void CFTPDlg::OnClickedRadioSer()
{
	// TODO: 在此添加控件通知处理程序代码
	m_radio = 0;

	UpdateData(FALSE);
}


void CFTPDlg::OnRadioCli()
{
	// TODO: 在此添加命令处理程序代码
	m_radio = 1;
	m_ipdress.SetAddress(127, 0, 0, 1);
	UpdateData(FALSE);
}


void CFTPDlg::insertlistctrl()
{
	// TODO: 在此处添加实现代码.
	m_filelist.DeleteAllItems();
	int index = 0;

	for (; index < MTU; index++)
	{
		if (list_filelist[index] == "")
		{
			break;
		}
		m_filelist.InsertItem(index, _T(""));//插入行
		m_filelist.SetItemText(index, 0, list_filelist[index]);//设置数据
	}
}


void CFTPDlg::OnClickedButtonSend()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);

	SOCKADDR_IN addrTo;
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = htons(6000);
	addrTo.sin_addr.S_un.S_addr = htonl(dwIP);

	CString strSend;
	GetDlgItemText(IDC_EDIT_MESSAGE, strSend);

	Packet FileData;
	FileData.flag = 6;


	char tempbuf[MTU] = { 0 };
	wsprintfA(tempbuf, "%ls", strSend);
	strcpy((char*)FileData.Data, tempbuf);
	if (m_radio == 1)
	{
		sendto(m_socket, (char*)&FileData, sizeof(FileData), 0, (SOCKADDR*)&addrTo, sizeof(SOCKADDR));
	}
	else
	{
		sendto(m_socket, (char*)&FileData, sizeof(FileData), 0, (SOCKADDR*)&addrFromC, sizeof(SOCKADDR));
	}
	char tempBuf2[MTU * 2];
	sprintf(tempBuf2, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrTo.sin_addr), tempbuf);
	CString mm(tempBuf2);

	m_log.AddString(mm);
	int nCount = m_log.GetCount();
	if (nCount > 0)
		m_log.SetCurSel(nCount - 1);
	SetDlgItemText(IDC_EDIT_MESSAGE, _T(""));

	
}


void CFTPDlg::OnClickedButtonSelFile()
{
	// TODO: 在此添加控件通知处理程序代码
	char szchar[Max];
	memset(szchar, 0, Max);

	Packet FileData;
	CFileDialog fileDlg(TRUE);//打开文件窗口	
	fileDlg.m_ofn.lpstrFilter = _T("所有文件\0*.*\0World文档(*.doc;*.docx)\0*doc;*.docx\0Excel表格(*.xls)\0*.xls\0文本文档(*.txt;*.pdf)\0*.txt;*.pdf\0图片(*.jpg)\0*.jpg\0压缩包(*.zip;*.rar)\0*.zip;*.rar\0\0");
	fileDlg.m_ofn.lpstrTitle = _T("打开文件");//定义打开对话框的标题
	if (fileDlg.DoModal() == IDOK)
	{
		file.Open(fileDlg.m_ofn.lpstrFile, CFile::modeRead);
		CString filename = fileDlg.m_ofn.lpstrFileTitle;//获取文件名
		wsprintfA(FileData.filename, "%ls", filename);
		long g_FileLength = file.GetLength();//获取文件的长度
		//计算包的总数
		FileData.Total = g_FileLength / MTU - 1;
		if (g_FileLength % MTU != 0)
		{
			FileData.Total = g_FileLength / MTU;
		}
		FileData.Number = 0;
		FileData.flag = 1;
		FileData.length = g_FileLength;
		FileData.Totallength = g_FileLength;

		DWORD dwIP;
		((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);

		SOCKADDR_IN addrTo;
		addrTo.sin_family = AF_INET;
		addrTo.sin_port = htons(6000);
		addrTo.sin_addr.S_un.S_addr = htonl(dwIP);

		char tempBuf[MTU * 2];
		strcpy((char*)FileData.Data, "请求向服务器上传文件");
		sprintf(tempBuf, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrTo.sin_addr), FileData.Data);

		sendto(m_socket, (char*)&FileData, sizeof(FileData), 0, (SOCKADDR*)&addrTo, sizeof(SOCKADDR));
		CString strt(tempBuf);

		m_log.AddString(strt);
		int nCount = m_log.GetCount();
		if (nCount > 0)
			m_log.SetCurSel(nCount - 1);
	}
}



void CFTPDlg::OnClickedButtonViewfile()
{
	// TODO: 在此添加控件通知处理程序代码
	Packet FileData;
	FileData.flag = 3;
	strcpy((char*)FileData.Data, "请求文件列表");

	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);

	SOCKADDR_IN addrTo;
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = htons(6000);
	addrTo.sin_addr.S_un.S_addr = htonl(dwIP);

	sendto(m_socket, (char*)&FileData, sizeof(FileData), 0, (SOCKADDR*)&addrTo, sizeof(SOCKADDR));
	char tempBuf[MTU * 2];
	sprintf(tempBuf, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrTo.sin_addr), FileData.Data);
	CString strt(tempBuf);

	m_log.AddString(strt);
	int nCount = m_log.GetCount();
	if (nCount > 0)
		m_log.SetCurSel(nCount - 1);
}


void CFTPDlg::OnClickedButtonDowlfile()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	if (IndexList == -1)
	{
		MessageBox(_T("请选择一个文件"));
		return;
	}

	CString needfilename = m_filelist.GetItemText(IndexList, 0);

	Packet FileData;
	FileData.flag = 4;
	memset(FileData.filename, 0, 100);
	wsprintfA(FileData.filename, "%ls", needfilename);
	needfilename = _T("请求下载文件: ") + needfilename;
	char tempbuf[MTU * 2] = { 0 };
	wsprintfA(tempbuf, "%ls", needfilename);
	strcpy((char*)FileData.Data, tempbuf);

	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);

	SOCKADDR_IN addrTo;
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = htons(6000);
	addrTo.sin_addr.S_un.S_addr = htonl(dwIP);

	sendto(m_socket, (char*)&FileData, sizeof(FileData), 0, (SOCKADDR*)&addrTo, sizeof(SOCKADDR));
	char tempBuf[MTU * 2];
	sprintf(tempBuf, "向IP为：%s发送消息:\r\n%s", inet_ntoa(addrTo.sin_addr), FileData.Data);
	CString strt(tempBuf);

	m_log.AddString(strt);
	int nCount = m_log.GetCount();
	if (nCount > 0)
		m_log.SetCurSel(nCount - 1);
}


void CFTPDlg::OnClickListFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	IndexList = pNMListView->iItem;

	*pResult = 0;
}


// 获取文件夹下的文件
void CFTPDlg::getFileList(CString szPath)
{
	// TODO: 在此处添加实现代码.
	m_filelist.DeleteAllItems();
	CFileFind finder;
	//建立CString对象，设置检索匹配字符串
	CString strWildcard(szPath);
	strWildcard += _T("\\*.*");

	//文件内部检索
	BOOL bWorking = finder.FindFile(strWildcard);
	int i; //用于定位字符，作为下标
	int index = 0;    //用于列表索引

	CString fontName, lastType, filepath, filename, filesize;//存储切割结果
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		filename = finder.GetFileName();
		long imageSize = finder.GetLength();
		if (filename != _T(".") && filename != _T(".."))
		{
			filepath = finder.GetFilePath();
			for (i = 0; i < filename.GetLength(); i++)
			{
				if (filename[i] == '.')
				{
					fontName = filename.Mid(0, i);
					lastType = filename.Mid(i + 1, filename.GetLength());

					CString filen;
					filen.Format(_T("%s%s%s"),fontName, _T("."), lastType);
					m_filelist.InsertItem(index, filen);
					map_filelist[mapindex++] = filen;
					
				}
			}
			index++;
		}
	
	}
	finder.Close();
}
