
// MailServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MailServer.h"
#include "MailServerDlg.h"
#include "afxdialogex.h"
#include "ListenSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CMailServerDlg �Ի���



CMailServerDlg::CMailServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMailServerDlg::IDD, pParent)
	, m_filename(_T("���޸���"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMailServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, mc_server);
	DDX_Control(pDX, IDC_LIST2, mc_mail);
	DDX_Control(pDX, IDC_LIST3, mc_text);
	DDX_Control(pDX, IDC_PICTURE, mc_picture);
	DDX_Text(pDX, IDC_EDIT1, m_filename);
}

BEGIN_MESSAGE_MAP(CMailServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LIST2, &CMailServerDlg::OnLbnSelchangeList2)
END_MESSAGE_MAP()


// CMailServerDlg ��Ϣ�������

BOOL CMailServerDlg::OnInitDialog()
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

	ShowWindow(SW_MINIMIZE);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	mc_server.SetHorizontalExtent(1000);
	mc_mail.SetHorizontalExtent(1000);
	mc_text.SetHorizontalExtent(1000);

	if(m_srvrSocket.m_hSocket ==INVALID_SOCKET){

		//���������׽���,����FD_ACCEPT�¼�,��TCP��25�˿��غ�,
		BOOL bFlag =m_srvrSocket.Create(25,SOCK_STREAM,FD_ACCEPT);
		if(!bFlag)  
        {  
			MessageBox(_T("Socket����ʧ��!"),L"����",MB_OK|MB_ICONSTOP);
            m_srvrSocket.Close();   
            return TRUE;  
        }  
        
		mc_server.AddString(_T("***SMTP������׼����"));
		mc_server.AddString(_T("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"));

		//�����ɹ�,�ȴ���������  
		if(!m_srvrSocket.Listen())//�������ʧ�� 
		{
			int nErrorCode =m_srvrSocket.GetLastError();    //��������Ϣ
			if(nErrorCode !=WSAEWOULDBLOCK)             //��������߳�����
			{  
				MessageBox(_T("Socket����!"),L"����",MB_OK|MB_ICONSTOP);
                m_srvrSocket.Close();  
                return TRUE;  
            }  
		}
	}


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMailServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMailServerDlg::OnPaint()
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
HCURSOR CMailServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMailServerDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//�ر��׽���
	m_srvrSocket.Close();

	CDialogEx::OnClose();
}


void CMailServerDlg::OnLbnSelchangeList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
