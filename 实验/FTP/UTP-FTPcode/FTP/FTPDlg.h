
// FTPDlg.h: 头文件
//
#include<map>
#include<iostream>
#include<string>
#pragma once

using namespace std;
#define	WM_RECVDATA		WM_USER+1
#define Max 1025
#define MTU 1024
#define T 2
#define FILENAMELEN 99
#define FILENUM 10

// CFTPDlg 对话框
class CFTPDlg : public CDialogEx
{
// 构造
public:
	CFTPDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FTP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonRestart();
//	CButton m_radio;
	CIPAddressCtrl m_ipdress;
	CListCtrl m_filelist;
//	BOOL m_radio;
	CListBox m_log;
	string str;
	// 初始化文件列表
	void initlistctrl();
	// 创建套接字
	bool InitSocket();
	// 绑定套接字端口
	void BindSocket();
	afx_msg void OnClickedButtonStart();
	static DWORD WINAPI RecvProc(LPVOID lpParameter);
	int m_radio;
	afx_msg void OnClickedRadioSer();
	afx_msg void OnRadioCli();
	void insertlistctrl();
	afx_msg void OnClickedButtonSend();
	afx_msg void OnClickedButtonSelFile();
	afx_msg void OnClickedButtonViewfile();
	afx_msg void OnClickedButtonDowlfile();
	afx_msg void OnClickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	// 获取文件夹下的文件
	void getFileList(CString szPath);
};
typedef struct _FileName
{
	char filename[FILENAMELEN];

}FileName;

typedef struct _Packet
{
	int flag;
	long Number;
	long Total;
	char filename[100];
	BYTE Data[MTU];
	int length;
	int Totallength;
	FileName mfilelist[FILENUM];

	_Packet()
	{
		flag = 0;
		Number = 0;
		Total = 0;
		memset(filename, 0, 0);
		for (int mindex = 0; mindex < MTU; mindex++)
		{
			Data[mindex] = 0;
		}
		length = 0;
		Totallength = 0;
		for (int mindex = 0; mindex < FILENUM; mindex++)
		{
			memset(mfilelist[mindex].filename, 0, FILENAMELEN);
		}
	}

	void init()
	{
		flag = 0;
		Number = 0;
		Total = 0;
		memset(filename, 0, 0);
		for (int mindex = 0; mindex < MTU; mindex++)
		{
			Data[mindex] = 0;
		}
		length = 0;
		Totallength = 0;
		for (int mindex = 0; mindex < FILENUM; mindex++)
		{
			memset(mfilelist[mindex].filename, 0, FILENAMELEN);
		}
	}

	void initdata()
	{
		for (int mindex = 0; mindex < MTU; mindex++)
		{
			Data[mindex] = 0;
		}
	}

	void initfilelist()
	{
		for (int mindex = 0; mindex < FILENUM; mindex++)
		{
			memset(mfilelist[mindex].filename, 0, FILENAMELEN);
		}
	}

}Packet;