// ListenSocket.cpp: 实现文件
//

#include "pch.h"
#include "SMTPServer.h"
#include "ListenSocket.h"
#include"MySocket.h"
#include"SMTPServerDlg.h"



// ListenSocket

ListenSocket::ListenSocket()
{
}

ListenSocket::~ListenSocket()
{
}


// ListenSocket 成员函数


void ListenSocket::OnAccept(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CSMTPServerDlg* dlg = (CSMTPServerDlg*)theApp.m_pMainWnd;
	dlg->m_log.AddString(_T("***收到连接请求"));
	dlg->m_log.AddString(_T("***建立连接"));
	MySocket* pSocket = new MySocket();
	if (Accept(*pSocket))
	{
		pSocket->AsyncSelect(FD_WRITE);
	}

	CAsyncSocket::OnAccept(nErrorCode);
}
