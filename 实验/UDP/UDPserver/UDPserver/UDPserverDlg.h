
// UDPserverDlg.h: 头文件
//

#pragma once
#include"MySocket.h"

// CUDPserverDlg 对话框
class CUDPserverDlg : public CDialogEx
{
// 构造
// 构造
public:
	CUDPserverDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPSERVER_DIALOG };
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
//	afx_msg void OnEnChangeEdit2();
	CString m_date;
//	CString m_ip;
	int m_port;
	CString m_time;
//	CString m_log;
//	CEdit m_log;
	void update_date_and_time(CString& date, CString& time);
	CListBox m_log;
//	CComboBox m_ip;
	CComboBox m_conip;
	CString m_ip;
	MySocket m_mysocket;
	afx_msg void OnClose();
};
