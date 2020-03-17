// MySocket.cpp: 实现文件
//

#include "pch.h"
#include "UDPserver.h"
#include "MySocket.h"
#include "UDPserverDlg.h"

// MySocket

MySocket::MySocket()
{
}

MySocket::~MySocket()
{
}


// MySocket 成员函数


void MySocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CUDPserverDlg* dig = (CUDPserverDlg*)theApp.m_pMainWnd;//获取窗口句柄
	TCHAR buff[4096];
	CString GetIp;//客户端的IP
	UINT GetPort;//客户端的端口号
	ReceiveFrom(buff, sizeof(buff), GetIp, GetPort);//接受发回的数据存在各个变量里。这个为客户端发来的请求数据
	CString data(buff);
	CString temp = data;
	dig->update_date_and_time(dig->m_date, dig->m_time);//把当前信息赋值给date&time

	CString Answer = _T("错误请求");
	data.MakeLower();//不管大小写全部置为小写

	if (data == "time")
		Answer = dig->m_time;//结果为当前时间
	if (data == "date")
		Answer = dig->m_date;//结果为当前日期

	SendToEx(Answer.GetBuffer(), (Answer.GetLength() + 1) * sizeof(WCHAR), GetPort, GetIp);//再把处理后的结果发回客户端
	CString record;
	record.Format(L"：收到IP=%s，Port=%u请求【%s】,响应【%s】", GetIp, GetPort, temp, Answer);//把收到的信息进行格式化，s为string，u为UINT
	dig->m_log.AddString(dig->m_date + _T(" ") + dig->m_time + record);//把记录发送到listbox
	CAsyncSocket::OnReceive(nErrorCode);
}
