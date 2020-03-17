#pragma once

// ListenSocket 命令目标
#include"MySocket.h"
class ListenSocket : public CAsyncSocket
{
public:
	ListenSocket();
	virtual ~ListenSocket();
	virtual void OnAccept(int nErrorCode);
	MySocket* m_pSocket;
};


