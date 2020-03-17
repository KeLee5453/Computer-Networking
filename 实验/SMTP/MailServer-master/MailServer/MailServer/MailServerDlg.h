
// MailServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "ListenSocket.h"

// CMailServerDlg �Ի���
class CMailServerDlg : public CDialogEx
{
// ����
public:
	CMailServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MAILSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox mc_server;
	CListBox mc_mail;
	ListenSocket m_srvrSocket; //�����׽���  
	afx_msg void OnClose();
	CListBox mc_text;
	afx_msg void OnLbnSelchangeList2();
	CStatic mc_picture;
	CString m_filename;
};
