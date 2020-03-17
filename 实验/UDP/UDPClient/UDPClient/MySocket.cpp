// MySocket.cpp: 实现文件
//

#include "pch.h"
#include "UDPClient.h"
#include "MySocket.h"
#include"UDPClientDlg.h"

// MySocket

MySocket::MySocket()
{

	GetBuff = _T("");
	GetIp = _T("");
	GetPort = 0;
}

MySocket::~MySocket()
{
}


// MySocket 成员函数



void MySocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	TCHAR buff[4096];
	//接受服务器发回的数据，放入缓冲区，数据的长度，
	ReceiveFrom(buff, sizeof(buff), GetIp, GetPort);
	CString data(buff);//数据赋值给data；
	GetBuff = data;//传给全局变量
	CUDPClientDlg* dig = (CUDPClientDlg*)theApp.m_pMainWnd;//获取窗口的句柄
	dig->m_reponse = GetBuff;//然后把值改为服务器相应的值
	dig->UpdateData(false);//不更新数据了。


	CAsyncSocket::OnReceive(nErrorCode);
}
