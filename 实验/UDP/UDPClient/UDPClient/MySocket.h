#pragma once

// MySocket 命令目标

class MySocket : public CAsyncSocket
{
public:
	MySocket();
	virtual ~MySocket();
//	virtual void OnAccept(int nErrorCode);
//	virtual void OnClose(int nErrorCode);
//	virtual void OnConnect(int nErrorCode);
//	virtual void OnSend(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
//	MySocket m_mysocket;
	CString GetBuff;
	CString GetIp;
	UINT GetPort;
};


