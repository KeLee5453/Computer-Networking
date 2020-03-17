
// SMTPServerDlg.h: 头文件
//

#pragma once
#include"ListenSocket.h"

// CSMTPServerDlg 对话框
class CSMTPServerDlg : public CDialogEx
{
// 构造
public:
	CSMTPServerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SMTPSERVER_DIALOG };
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
//	CListBox m_picture;
//	CListBox m_content;
	CListBox m_log;
//	CListBox m_list;
//	CListBox m_text;
//	CStatic m_picture;
//	CString m_filename;
	CListBox mc_content;
	CListBox mc_text;
	CString mc_filename;
	ListenSocket m_srvrSocket;
	afx_msg void OnClose();
	CListBox mc_subject;
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedRestart();
//	CStatic m_picture;
	CStatic m_picture;
};
