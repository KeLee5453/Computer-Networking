// ListenSocket.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MailServer.h"
#include "ListenSocket.h"
#include "MySocket.h"
#include "MailServerDlg.h"

// ListenSocket

ListenSocket::ListenSocket()
{
}

ListenSocket::~ListenSocket()
{
}


// ListenSocket ��Ա����


void ListenSocket::OnAccept(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CMailServerDlg* dlg = (CMailServerDlg*)theApp.m_pMainWnd; //��ȡ�Ի�����

	dlg->mc_server.AddString(_T("***�յ���������"));
	dlg->mc_server.AddString(_T("***��������"));
	
	
	MySocket *pSocket =new MySocket();  

	if(Accept(*pSocket)){
		pSocket->AsyncSelect(FD_WRITE); //����OnSend����
	}

	CAsyncSocket::OnAccept(nErrorCode);
}
